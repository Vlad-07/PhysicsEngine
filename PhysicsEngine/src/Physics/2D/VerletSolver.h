#pragma once

#include <Eis.h>

#include "VerletObject.h"

class VerletSolver
{
public:
	VerletSolver();
	~VerletSolver() = default;

	void Update(Eis::TimeStep ts);
	void UpdatePositions(Eis::TimeStep ts);
	void ApplyGravity();
	void Constraint();
	void SolveColisions();

	bool CheckCollision(glm::vec2 pos, float radius);

	void AddObject(glm::vec2 pos = glm::vec2(0.0f), float diameter = 1.0f, glm::vec2 accel = glm::vec2(0.0f)) { m_PhysicsObjectPool.push_back(VerletObject(pos, diameter, accel)); }
	void AddObject(const VerletObject& obj) { m_PhysicsObjectPool.push_back(obj); }
	void ClearObjects() { m_PhysicsObjectPool.clear(); }

	inline float GetConstraintRadius() const { return m_ConstraintRadius; }
	inline glm::vec2 GetGravity() const { return m_Gravity; }
	inline std::vector<VerletObject> GetObjectPool() const { return m_PhysicsObjectPool; }

	void SetConstraintRadius(float radius) { m_ConstraintRadius = radius; }
	void SetGravity(glm::vec2 gravity) { m_Gravity = gravity; }

private:
	std::vector<VerletObject> m_PhysicsObjectPool;
	glm::vec2 m_Gravity;
	float m_ConstraintRadius;

	int m_SubSteps;
};