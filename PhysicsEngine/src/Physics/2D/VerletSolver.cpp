#include "VerletSolver.h"


VerletSolver::VerletSolver() : m_Gravity(glm::vec2(0.0f, -50.0f)), m_ConstraintDimensions(25.0f), m_Collisions(true)
{}

void VerletSolver::Update(Eis::TimeStep ts)
{
	ApplyGravity();
	UpdateChains();
	Constraint();
	FindColisions_SLOW();
	UpdatePositions(ts);
}

void VerletSolver::UpdateSubStepped(Eis::TimeStep ts, int subSteps)
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
	if (!m_Collisions)
		return;

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

bool VerletSolver::CheckCollision(const VerletObject& obj1, const VerletObject& obj2)
{
	const glm::vec2 collisionAxis = obj1.GetPosition() - obj2.GetPosition();
	const float dist = glm::length(collisionAxis);
	const float minDist = obj1.GetRadius() + obj2.GetRadius();

	return dist < minDist;
}