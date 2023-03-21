#pragma once

#include <Eis.h>

#include "VerletObject.h"
#include "Chain.h"

class VerletSolver
{
public:
	VerletSolver();
	~VerletSolver() = default;

	void Update(Eis::TimeStep ts);
	void UpdatePositions(Eis::TimeStep ts);
	void UpdateChains();
	void ApplyGravity();
	void Constraint();
	void SolveColisions();

	bool CheckCollision(glm::vec2 pos, float diameter);

	void AddObject(glm::vec2 pos = glm::vec2(0.0f), float diameter = 1.0f, glm::vec2 accel = glm::vec2(0.0f)) { m_PhysicsObjectPool.push_back(VerletObject(pos, diameter, accel)); }
	void AddObject(const VerletObject& obj) { m_PhysicsObjectPool.push_back(obj); }
	void ClearObjects() { m_PhysicsObjectPool.clear(); }
	void ClearChainLinks() { m_ChainLinkPool.clear(); }

	void AddChainLink(int objId1, int objId2, float dist = 0.0f) { m_ChainLinkPool.push_back(ChainLink(objId1, objId2, m_PhysicsObjectPool, dist)); }
	void AddChainLink(ChainLink& chainLink) { m_ChainLinkPool.push_back(chainLink); }

	inline int GetSubsteps() { return m_SubSteps; }
	inline glm::vec2 GetGravity() const { return m_Gravity; }
	inline float GetConstraintRadius() const { return m_ConstraintRadius; }
	inline VerletObject& GetObjectRef(int id) { return m_PhysicsObjectPool[id]; }
	inline std::vector<VerletObject> GetObjectPool() const { return m_PhysicsObjectPool; }

	void SetConstraintRadius(float radius) { m_ConstraintRadius = radius; }
	void SetGravity(glm::vec2 gravity) { m_Gravity = gravity; }
	void SetSubsteps(int substeps) { m_SubSteps = substeps; }

private:
	std::vector<VerletObject> m_PhysicsObjectPool;
	std::vector<ChainLink> m_ChainLinkPool;
	glm::vec2 m_Gravity;
	float m_ConstraintRadius;

	int m_SubSteps;
};