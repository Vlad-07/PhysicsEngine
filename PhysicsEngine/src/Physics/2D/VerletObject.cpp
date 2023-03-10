#include "VerletObject.h"

void VerletObject::UpdatePosition(Eis::TimeStep ts)
{
	const glm::vec2 velocity = m_Position - m_LastPosition;

	m_LastPosition = m_Position;

	m_Position = m_Position + velocity + m_Acceleration * (ts.GetSeconds() * ts.GetSeconds());

	m_Acceleration = glm::vec2(0.0f);
}

void VerletObject::Accelerate(glm::vec2 accel)
{
	m_Acceleration += accel;
}