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

	void Interactions();
	void RenderPhysicsObjects() const;

private:
	Eis::OrthoCameraController m_CameraController;
	VerletSolver m_PhysicsSolver;
	Eis::Ref<Eis::Texture2D> m_CircleTexture;

	glm::vec2 m_PreviewPos;
	float m_PreviewDiameter;

	bool m_Flood;

	glm::vec3 m_ColorMem[500];
	Eis::Image m_Img;
};