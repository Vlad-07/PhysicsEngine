#pragma once

#include <Eis.h>

#include <glm/glm.hpp>

#include "VerletObject.h"

class ChainLink
{
public:
	ChainLink() = default;
	ChainLink(int obj1id, int obj2id, std::vector<VerletObject>& pool, float targetDist);
	~ChainLink() = default;

	void Apply();

	float GetTargetDistance() const { return m_TargetDistance; }

	void SetTargetDistance(float targetDist) { m_TargetDistance = targetDist; }

private:
	std::vector<VerletObject>& m_PhysObjPool;
	int m_Obj1Id;
	int m_Obj2Id;
	float m_TargetDistance;
};