#include "VerletSolver.h"


VerletSolver::VerletSolver()
	: m_Gravity(glm::vec2(0.0f, -50.0f)), m_ConstraintDimensions(25.0f), m_Collisions(true), m_EnableGridPartitioning(true), m_Pause(false)
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
	// Pause and step management
	{
		static uint8_t safeguard = 0b11;
		if (Eis::Input::IsKeyPressed(EIS_KEY_P))
		{
			if (safeguard & BIT(0))
				m_Pause = !m_Pause, safeguard ^= BIT(0);
		}
		else
			safeguard |= BIT(0);

		if (Eis::Input::IsKeyPressed(EIS_KEY_O))
		{
			if (safeguard & BIT(1))
			{
				safeguard ^= BIT(1);
				m_Pause = true;
				goto _Step;
			}
		}
		else
			safeguard |= BIT(1);

		if (m_Pause)
			return;

	_Step:;
	}


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
		obj.Accelerate(m_Gravity * (float)obj.GetGravityInfl());
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

	const float massMod1 = -obj1.GetMass() / (obj1.GetMass() + obj2.GetMass());
	const float massMod2 =  obj2.GetMass() / (obj1.GetMass() + obj2.GetMass());

	obj1.AddPosition(massMod2 * delta * n);
	obj2.AddPosition(massMod1 * delta * n);
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

void VerletSolver::RenderPhysicsObjects() const
{
	for (const VerletObject& obj : m_PhysicsObjectPool)
		Eis::Renderer2D::DrawCircle(obj.GetPosition(), glm::vec2(obj.GetDiameter()), glm::vec4(obj.GetColor(), 1.0f));
}

void VerletSolver::DebugGrid() const
{
	for (int y = 0; y < m_Grid.GetHeight(); y++)
	{
		for (int x = 0; x < m_Grid.GetWidth(); x++)
		{
			glm::vec4 color;
			if (m_Grid[y][x].objects.size())
				color = glm::vec4(0.2f, 0.8f, 0.2f, 0.8f);
			else
				color = glm::vec4(0.06f, 0.06f, 0.06f, 0.5f);

			Eis::Renderer2D::DrawQuad({ x * 2 - m_ConstraintDimensions.x + 1.0f, y * 2 - m_ConstraintDimensions.y + 1.0f }, glm::vec2(2.0f), color);
		}
	}
}