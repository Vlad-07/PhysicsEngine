#pragma once

#include <glm/glm.hpp>
#include <Eis/Core/TimeStep.h>

class VerletObject
{
public:
	VerletObject() = default;
	VerletObject(uint32_t id, glm::vec2 pos, float rotation = 0.0f, float diameter = 1.0f, glm::vec2 accel = glm::vec2(0.0f));
	virtual ~VerletObject() = default;

	void UpdatePosition(Eis::TimeStep ts);
	void Accelerate(glm::vec2 accel);

	inline glm::vec2 GetPosition() const { return m_Position; }
	inline glm::vec2 GetLastPosition() const { return m_LastPosition; }
	inline glm::vec2 GetAcceleration() const { return m_Acceleration; }
	inline glm::vec2 GetVelocity() const { return m_Position - m_LastPosition; }
	inline glm::vec3 GetColor() const { return m_Color; }
	inline float GetRotation() const { return m_Rotation; }
	inline float GetDiameter() const { return m_Diameter; }
	inline float GetRadius() const { return m_Diameter / 2.0f; }
	inline float GetMass() const { return m_Mass; }
	inline bool GetGravityInfl() const { return m_AffectedByGravity; }
	inline uint32_t GetId() const { return m_Id; }

	void SetId(uint32_t id) { m_Id = id; }
	void SetColor(glm::vec3 color) { m_Color = color; }
	void SetMass(float mass) { m_Mass = mass; }
	void SetRotation(float rotation) { m_Rotation = rotation; }
	void SetGravityInfluence(bool affectedByGravity) { m_AffectedByGravity = affectedByGravity; }
	void SetSlowdown(float slowFactor) { m_Slowdown = slowFactor; }
	void SetPosition(glm::vec2 pos) { if (!m_Locked) m_Position = pos; }
	void AddPosition(glm::vec2 mov) { if (!m_Locked) m_Position += mov; }
	void StopMovement() { m_LastPosition = m_Position; m_Acceleration = glm::vec2(0.0f); } // TODO: stop movement not working

	void Lock() { m_Locked = true; }
	void Unlock() { m_Locked = false; }

	bool operator ==(const VerletObject& obj) const { return (m_Id == obj.GetId()); }
	bool operator !=(const VerletObject& obj) const { return (m_Id != obj.GetId()); }

private:
	glm::vec2 m_Position;
	glm::vec2 m_LastPosition;
	glm::vec2 m_Acceleration;
	glm::vec3 m_Color;
	float m_Rotation; // rads
	float m_Diameter;
	float m_Mass;
	float m_Slowdown;
	bool m_Locked;
	bool m_AffectedByGravity;

	uint32_t m_Id;
};