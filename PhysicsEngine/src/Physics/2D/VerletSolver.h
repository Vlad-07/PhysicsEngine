#pragma once

#include <Eis.h>

#include "VerletObject.h"
#include "Chain.h"
#include "Grid.h"

// TODO: standardize diameter instead of radius?

class VerletSolver
{
public:
	VerletSolver();
	~VerletSolver() = default;

	void Update(Eis::TimeStep ts);
	void UpdateSubStepped(Eis::TimeStep ts, int subSteps);
	void UpdateGrid();

	void AddObject(glm::vec2 pos = glm::vec2(0.0f), float diameter = 1.0f, glm::vec2 accel = glm::vec2(0.0f)) { m_PhysicsObjectPool.push_back(VerletObject(m_PhysicsObjectPool.size(), pos, diameter, accel)); }
	void AddObject(const VerletObject& obj) { m_PhysicsObjectPool.push_back(obj); }
	void ClearObjects() { m_PhysicsObjectPool.clear(); }

	void AddChainLink(int objId1, int objId2, float dist = 0.0f) { m_ChainLinkPool.push_back(ChainLink(objId1, objId2, m_PhysicsObjectPool, dist)); }
	void AddChainLink(ChainLink& chainLink) { m_ChainLinkPool.push_back(chainLink); }
	void ClearChainLinks() { m_ChainLinkPool.clear(); }

	bool CheckCollision(glm::vec2 pos, float radius);
	bool CheckCollision(const VerletObject& obj1, const VerletObject& obj2);

	void SolveNANs(); // For bringing back objects from the backrooms;

	inline auto GetGravity() const { return m_Gravity; }
	inline auto GetConstraintDimensions() const { return m_ConstraintDimensions; }
	inline auto GetObjectPool() const { return m_PhysicsObjectPool; }
	inline auto GetGrid() const { return m_Grid; }
	inline auto& GetObjectRef(int id) { return m_PhysicsObjectPool[id]; }
	inline auto GetCollisionDetection() const { return m_Collisions; }
	inline auto GetGridStatus() const { return m_EnableGridPartitioning; }

	inline void SetConstraintDimensions(glm::vec2 radius) { m_ConstraintDimensions = radius; }
	inline void SetGravity(glm::vec2 gravity) { m_Gravity = gravity; }
	inline void SetCollisionDetection(bool collisions) { m_Collisions = collisions; }
	inline void SetGridStatus(bool enabled) { m_EnableGridPartitioning = enabled; }

private:
	void UpdatePositions(Eis::TimeStep ts);
	void UpdateChains();
	void ApplyGravity();
	void Constraint();

	void FindColisions_SLOW();

	void FindCollisionsGrid();
	void CheckGridCellColisions(const GridCell& cell1, const GridCell& cell2);

	void SolveCollision(VerletObject& obj1, VerletObject& obj2);

private:
	std::vector<VerletObject> m_PhysicsObjectPool;
	std::vector<ChainLink> m_ChainLinkPool;
	std::vector<std::vector<GridCell>> m_Grid;
	bool m_EnableGridPartitioning;

	glm::vec2 m_Gravity;
	glm::vec2 m_ConstraintDimensions;
	bool m_Collisions;
};