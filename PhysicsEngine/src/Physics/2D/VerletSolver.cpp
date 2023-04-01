#include "VerletSolver.h"


VerletSolver::VerletSolver()
	: m_Gravity(glm::vec2(0.0f, -50.0f)), m_ConstraintDimensions(25.0f), m_Collisions(true), m_EnableGridPartitioning(true)
{
	m_Grid.SetGridSize((int)m_ConstraintDimensions.x, (int)m_ConstraintDimensions.y);
}

void VerletSolver::Update(Eis::TimeStep ts)
{
	SolveNANs();
	ApplyGravity();
	UpdateChains();
	Constraint();
	if (m_Collisions)
	{
		if (m_EnableGridPartitioning)
		{
			UpdateGrid();
			FindCollisionsGrid();
		}
		else
			FindColisions_SLOW();
	}
	UpdatePositions(ts);
}

void VerletSolver::UpdatePhysics(Eis::TimeStep ts, int subSteps)
{
	Eis::TimeStep subTs(ts.GetSeconds() / subSteps);
	for (; subSteps; subSteps--)
		Update(subTs);
}

void VerletSolver::UpdatePositions(Eis::TimeStep ts)
{
	for (VerletObject& obj : m_PhysicsObjectPool)
		obj.UpdatePosition(ts);
}

void VerletSolver::UpdateChains()
{
	for (ChainLink& link : m_ChainLinkPool)
		link.Apply();
}

void VerletSolver::ApplyGravity()
{
	for (VerletObject& obj : m_PhysicsObjectPool)
		obj.Accelerate(m_Gravity);
}

void VerletSolver::Constraint()
{	
	for (VerletObject& obj : m_PhysicsObjectPool)
	{
		const float objRadius = obj.GetDiameter() / 2.0f;

		if (obj.GetPosition().x < -m_ConstraintDimensions.x + objRadius)
			obj.SetPosition({ -m_ConstraintDimensions.x + objRadius, obj.GetPosition().y });
		else if (obj.GetPosition().x > m_ConstraintDimensions.x - objRadius)
			obj.SetPosition({ m_ConstraintDimensions.x - objRadius, obj.GetPosition().y });

		if (obj.GetPosition().y < -m_ConstraintDimensions.y + objRadius)
			obj.SetPosition({ obj.GetPosition().x, -m_ConstraintDimensions.y + objRadius });
		else if (obj.GetPosition().y > m_ConstraintDimensions.y - objRadius)
			obj.SetPosition({ obj.GetPosition().x, m_ConstraintDimensions.y - objRadius });
	}
}

void VerletSolver::FindColisions_SLOW()
{
	for (uint32_t i = 0; i < m_PhysicsObjectPool.size(); i++)
	{
		VerletObject& obj1 = m_PhysicsObjectPool[i];
		for (uint32_t j = i + 1; j < m_PhysicsObjectPool.size(); j++)
		{
			VerletObject& obj2 = m_PhysicsObjectPool[j];

			if (CheckCollision(obj1, obj2))
				SolveCollision(obj1, obj2);
		}
	}
}

void VerletSolver::UpdateGrid()
{
	m_Grid.FlushGrid();

	for (int i = 0; i < m_PhysicsObjectPool.size(); i++)
	{
		const VerletObject& obj = m_PhysicsObjectPool[i];

		int x = (int)((obj.GetPosition().x + m_ConstraintDimensions.x) / 2.0f);
		int y = (int)((obj.GetPosition().y + m_ConstraintDimensions.y) / 2.0f);

		m_Grid[y][x].objects.push_back(i);
	}
}

void VerletSolver::FindCollisionsGrid()
{
	for (uint32_t y = 0; y < m_Grid.GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_Grid.GetWidth(); x++)
		{
			const GridCell& currentCell = m_Grid[y][x];

			for (int32_t dy = -1; dy <= 1; dy++)
			{
				for (int32_t dx = -1; dx <= 1; dx++)
				{
					if (y + dy < 0 || x + dx < 0 || y + dy >= m_Grid.GetHeight() || x + dx >= m_Grid.GetWidth())
						continue;

					const GridCell& otherCell = m_Grid[y + dy][x + dx];
					CheckGridCellColisions(currentCell, otherCell);
				}
			}
		}
	}
}

void VerletSolver::CheckGridCellColisions(const GridCell& cell1, const GridCell& cell2)
{
	for (uint32_t i = 0; i < cell1.objects.size(); i++)
	{
		VerletObject& obj1 = m_PhysicsObjectPool[cell1.objects[i]];
		for (uint32_t j = 0; j < cell2.objects.size(); j++)
		{
			VerletObject& obj2 = m_PhysicsObjectPool[cell2.objects[j]];

			if (CheckCollision(obj1, obj2) && obj1 != obj2)
				SolveCollision(obj1, obj2);
		}
	}
}

void VerletSolver::SolveCollision(VerletObject& obj1, VerletObject& obj2)
{
	const glm::vec2 collisionAxis = obj1.GetPosition() - obj2.GetPosition();
	const float dist = glm::length(collisionAxis);
	const glm::vec2 n = collisionAxis / dist;
	const float minDist = obj1.GetRadius() + obj2.GetRadius();
	const float delta = minDist - dist;
	obj1.AddPosition(0.5f * delta * n);
	obj2.AddPosition(-0.5f * delta * n);
}

bool VerletSolver::CheckCollision(glm::vec2 pos, float radius)
{
	for (uint32_t i = 0; i < m_PhysicsObjectPool.size(); i++)
	{
		VerletObject& obj = m_PhysicsObjectPool[i];
		const glm::vec2 collisionAxis = obj.GetPosition() - pos;
		const float dist = glm::length(collisionAxis);
		const float minDist = obj.GetRadius() + radius;

		if (dist < minDist)
			return true;
	}
	return false;
}
bool VerletSolver::CheckCollision(const VerletObject& obj)
{
	for (uint32_t i = 0; i < m_PhysicsObjectPool.size(); i++)
	{
		VerletObject& obj2 = m_PhysicsObjectPool[i];
		const glm::vec2 collisionAxis = obj.GetPosition() - obj2.GetPosition();
		const float dist = glm::length(collisionAxis);
		const float minDist = obj.GetRadius() + obj2.GetRadius();

		if (dist < minDist)
			return true;
	}
	return false;
}
bool VerletSolver::CheckCollision(const VerletObject& obj1, const VerletObject& obj2)
{
	const glm::vec2 collisionAxis = obj1.GetPosition() - obj2.GetPosition();
	const float dist = glm::length(collisionAxis);
	const float minDist = obj1.GetRadius() + obj2.GetRadius();

	return dist < minDist;
}

void VerletSolver::SolveNANs()
{
	for (VerletObject& obj : m_PhysicsObjectPool)
	{
		if (isnan(obj.GetPosition().x) || isnan(obj.GetPosition().y))
		{
			EIS_WARN("Nan position detected!");
			obj.SetPosition(glm::vec2(0.0f));
			obj.StopMovement();
		}
	}
}