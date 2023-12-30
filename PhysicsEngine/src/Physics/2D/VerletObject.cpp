#include "VerletObject.h"

VerletObject::VerletObject(uint32_t id, glm::vec2 pos, float rotation, float diameter, glm::vec2 accel)
	: m_Id(id), m_Position(pos), m_LastPosition(pos), m_Rotation(rotation), m_Diameter(diameter), m_Acceleration(accel),
	  m_Color(1.0f), m_Mass(1.0f), m_Slowdown(1.0f), m_Locked(false), m_AffectedByGravity(true)
{
}

void VerletObject::UpdatePosition(Eis::TimeStep ts)
{
	if (m_Locked)
		return;

	const glm::vec2 velocity = m_Position - m_LastPosition;

	m_LastPosition = m_Position;

	m_Position = m_Position + velocity * m_Slowdown + m_Acceleration * (ts.GetSeconds() * ts.GetSeconds());

	m_Acceleration = glm::vec2(0.0f);
}

void VerletObject::Accelerate(glm::vec2 accel)
{
	m_Acceleration += accel;
}