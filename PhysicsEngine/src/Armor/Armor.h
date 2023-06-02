#pragma once

#include <Eis.h>
#include <imgui.h>

#include "Physics/2D/VerletSolver.h"

// (un)realistic armor simulation

class Armor : public Eis::Layer
{
public:
	Armor();
	virtual ~Armor() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Eis::TimeStep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Eis::Event& e) override;

private:
	Eis::OrthoCameraController m_CameraController;
	VerletSolver m_PhysicsSolver;

	int m_PhysicsSubsteps;
	bool m_DebugGrid;

	bool m_ProjectileActive;

	glm::ivec2 m_ArmorSize;
	float m_ArmorRadius;
	float m_ArmorHardness;
	float m_ArmorResistance;
	float m_Slope;

	float m_ShellRadius;
};