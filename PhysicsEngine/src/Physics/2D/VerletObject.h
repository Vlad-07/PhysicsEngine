#pragma once

#include <glm/glm.hpp>
#include <Eis/Core/TimeStep.h>

class VerletObject
{
public:
	VerletObject() = default;
	VerletObject(uint32_t id, glm::vec2 pos, float diameter = 1.0f, glm::vec2 accel = glm::vec2(0.0f)) : m_Id(id), m_Position(pos), m_LastPosition(pos), m_Diameter(diameter), m_Acceleration(accel), m_Color(1.0f), m_Locked(false) {}
	~VerletObject() = default;

	void UpdatePosition(Eis::TimeStep ts);
	void Accelerate(glm::vec2 accel);

	inline glm::vec2 GetPosition() const { return m_Position; }
	inline glm::vec2 GetLastPosition() const { return m_LastPosition; }
	inline glm::vec2 GetAcceleration() const { return m_Acceleration; }
	inline glm::vec3 GetColor() const { return m_Color; }
	inline float GetDiameter() const { return m_Diameter; }
	inline float GetRadius() const { return m_Diameter / 2.0f; }
	inline uint32_t GetId() const { return m_Id; }

	void SetId(uint32_t id) { m_Id = id; }
	void SetColor(glm::vec3 color) { m_Color = color; }
	void SetPosition(glm::vec2 pos) { if (!m_Locked) m_Position = pos; }
	void AddPosition(glm::vec2 mov) { if (!m_Locked) m_Position += mov; }
	void StopMovement() { m_LastPosition = m_Position; m_Acceleration = glm::vec2(0.0f); } // TODO: stop movement not working

	void Lock() { m_Locked = true; }
	void Unlock() { m_Locked = false; }

	bool operator ==(const VerletObject& obj);
	bool operator !=(const VerletObject& obj);

private:
	glm::vec2 m_Position;
	glm::vec2 m_LastPosition;
	glm::vec2 m_Acceleration;
	glm::vec3 m_Color;
	float m_Diameter;
	bool m_Locked;

	uint32_t m_Id;
};