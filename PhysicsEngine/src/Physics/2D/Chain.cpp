#include "Chain.h"

#include <Eis/Core/Log.h>

ChainLink::ChainLink(int obj1, int obj2, std::vector<VerletObject>& pool = std::vector<VerletObject>(), float targetDist = 0.0f) : m_Obj1Id(obj1), m_Obj2Id(obj2), m_PhysObjPool(pool)
{
	m_TargetDistance = targetDist > 0.0f ? targetDist : ( m_PhysObjPool[obj1].GetRadius() + m_PhysObjPool[obj2].GetRadius() + 0.1f);
}

void ChainLink::Apply()
{
	VerletObject& obj1 = m_PhysObjPool[m_Obj1Id];
	VerletObject& obj2 = m_PhysObjPool[m_Obj2Id];

	const glm::vec2 axis = obj1.GetPosition() - obj2.GetPosition();
	const float dist = glm::length(axis);
	const glm::vec2 n = axis / dist;
	const float delta = m_TargetDistance - dist;

	obj1.AddPosition( 0.5f * delta * n);
	obj2.AddPosition(-0.5f * delta * n);
}