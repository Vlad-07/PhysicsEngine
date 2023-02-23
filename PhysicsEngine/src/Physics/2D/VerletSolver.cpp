#include "VerletSolver.h"


VerletSolver::VerletSolver() : m_Gravity(glm::vec2(0.0f, -10.0f)), m_SubSteps(8), m_ConstraintRadius(4.0f)
{}


void VerletSolver::Update(Eis::TimeStep ts)
{
	const Eis::TimeStep subTs(ts.GetSeconds() / m_SubSteps);
	for (uint32_t i = m_SubSteps; i; i--)
	{
		ApplyGravity();
		Constraint();
		SolveColisions();
		UpdatePositions(subTs);
	}
}

void VerletSolver::UpdatePositions(Eis::TimeStep ts)
{
	for (VerletObject& obj : m_PhysicsObjectPool)
		obj.UpdatePosition(ts);
}

void VerletSolver::ApplyGravity()
{
	for (VerletObject& obj : m_PhysicsObjectPool)
		obj.Accelerate(m_Gravity);
}

void VerletSolver::Constraint()
{
	const glm::vec2 center = glm::vec2(0.0f);

	for (VerletObject& obj : m_PhysicsObjectPool)
	{
		const glm::vec2 toObj = center - obj.GetPosition();
		const float distance = glm::length(toObj);
		const float objRadius = obj.GetRadius();
		if (distance > m_ConstraintRadius - objRadius)
		{
			const glm::vec2 n = toObj / distance;
			obj.SetPosition(center - n * (m_ConstraintRadius - objRadius));
		}
	}
}

void VerletSolver::SolveColisions()
{
	for (uint32_t i = 0; i < m_PhysicsObjectPool.size(); i++)
	{
		VerletObject& obj1 = m_PhysicsObjectPool[i];
		for (uint32_t j = i + 1; j < m_PhysicsObjectPool.size(); j++)
		{
			VerletObject& obj2 = m_PhysicsObjectPool[j];
			
			const glm::vec2 collisionAxis = obj1.GetPosition() - obj2.GetPosition();
			const float dist = glm::length(collisionAxis);
			const float minDist = obj1.GetRadius() + obj2.GetRadius();
			if (dist < minDist)
			{
				const glm::vec2 n = collisionAxis / dist;
				const float delta = minDist - dist;
				obj1.AddPosition( 0.5f * delta * n);
				obj2.AddPosition(-0.5f * delta * n);
			}
		}
	}
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