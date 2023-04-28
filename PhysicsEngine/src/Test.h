#pragma once

#include <Eis.h>

#include <imgui.h>

#include "Physics/2D/VerletSolver.h"
#include "Physics/2D/VerletObject.h"

#include <vector>

class Test : public Eis::Layer
{
public:
	Test();
	virtual ~Test() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Eis::TimeStep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Eis::Event& e) override;

	void RenderPhysicsObjects() const;
	void DebugGrid();

private:
	Eis::OrthoCameraController m_CameraController;
	VerletSolver m_PhysicsSolver;

	glm::vec2 m_PreviewPos;
	float m_PreviewDiameter;

	bool m_Flood;

	int m_ThreadCount;
	int m_PhysicsSubsteps;
	bool m_DebugGrid;

	glm::vec3 m_ColorMem[11500];
	Eis::Image m_Img;
};