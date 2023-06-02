#include "Armor.h"

Armor::Armor()
	: Layer(), m_CameraController(16.0f / 9.0f), m_DebugGrid(false), m_PhysicsSubsteps(8), m_ArmorHardness(0.5f),
	  m_ArmorResistance(1.0f), m_Slope(0.0f), m_ProjectileActive(false), m_ArmorSize(20, 100), m_ArmorRadius(0.5f)
{
}

void Armor::OnAttach()
{
	Eis::RenderCommands::Disable(0x0B71); // GL_DEPTH_TEST
	m_CameraController.OnEvent(Eis::MouseScrolledEvent(0.0f, -130.0f)); // HACK: artificial camera zoom

	m_PhysicsSolver.SetConstraintDimensions(glm::vec2(100.0f));
}

void Armor::OnDetach()
{
}

void Armor::OnUpdate(Eis::TimeStep ts)
{
	m_CameraController.OnUpdate(ts);

	Eis::RenderCommands::SetClearColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	Eis::RenderCommands::Clear();


	m_PhysicsSolver.UpdatePhysics(Eis::TimeStep(0.0136f), m_PhysicsSubsteps);


	Eis::Renderer2D::BeginScene(m_CameraController.GetCamera());

	// Background
	if (!m_DebugGrid) Eis::Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, -0.1f), m_PhysicsSolver.GetConstraintDimensions() * 2.0f, glm::vec4(0.06f, 0.06f, 0.06f, 0.5f));


	if (m_DebugGrid) m_PhysicsSolver.DebugGrid();
	m_PhysicsSolver.RenderPhysicsObjects();

	Eis::Renderer2D::EndScene();
}

void Armor::OnImGuiRender()
{
	ImGui::Begin("Simulation");

	// Instructions
	ImGui::Text("P - Toggle pause");
	ImGui::Text("O - Step");

	// Sim
	ImGui::Separator();

	if (ImGui::Button("Clear"))
	{
		m_PhysicsSolver.ClearObjects();
		m_PhysicsSolver.ClearChainLinks();
		m_ProjectileActive = false;
	}

	if (ImGui::Button("Spawn armor"))
	{
		m_PhysicsSolver.ClearObjects();
		m_PhysicsSolver.ClearChainLinks();
		m_ProjectileActive = false;

		int s = m_PhysicsSolver.GetObjectPool().size();

		for (float y = 0, slopeOff = -(m_Slope * (m_ArmorSize.y / 2.0f)); y < m_ArmorSize.y; y++, slopeOff += m_Slope)
			for (float x = 0; x < m_ArmorSize.x; x++)
				m_PhysicsSolver.AddObject(glm::vec2((x - m_ArmorSize.x / 2) * m_ArmorRadius + 0.001f + slopeOff, (y - m_ArmorSize.y / 2) * m_ArmorRadius + 0.001f), m_ArmorRadius);


		for (int i = s; i < m_PhysicsSolver.GetObjectPool().size(); i++)
		{
			m_PhysicsSolver.GetObjectRef(i).SetGravityInfluence(false);
			m_PhysicsSolver.GetObjectRef(i).SetSlowdown(1.0f - m_ArmorHardness);
			m_PhysicsSolver.GetObjectRef(i).SetMass(m_ArmorResistance);
		}
	}

	// Projectile
	static float maxX = -100.0f;
	if (ImGui::Button("Spawn projectile"))
	{
		if (m_ProjectileActive)
			m_PhysicsSolver.DeleteObject(m_PhysicsSolver.GetObjectPool().size() - 1);

		m_ProjectileActive = true;
		maxX = -100.0f;
		m_PhysicsSolver.AddObject(glm::vec2(-50.0f, 0.0f), m_ShellRadius, glm::vec2(300000.0f, 0.0f));
		m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).SetMass(20.0f);
		m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).SetGravityInfluence(false);
	}

	if (ImGui::Button("Despawn projectile") && m_ProjectileActive)
	{
		m_ProjectileActive = false;
		m_PhysicsSolver.DeleteObject(m_PhysicsSolver.GetObjectPool().size() - 1);
	}

	// Armor Stats
	ImGui::Separator();
	ImGui::SliderInt("Armor size X", &m_ArmorSize.x, 2, 50);
	ImGui::SliderInt("Armor size Y", &m_ArmorSize.y, 2, 100);
	ImGui::SliderFloat("Armor radius", &m_ArmorRadius, 0.2f, 1.0f);
	ImGui::SliderFloat("Hardness", &m_ArmorHardness, 0.1f, 1.0f);
	ImGui::SliderFloat("Resistance", &m_ArmorResistance, 0.1f, 10.0f);
	ImGui::SliderFloat("Slope", &m_Slope, 0.0f, 0.5f, "");

	// Shell stats
	ImGui::Separator();
	ImGui::SliderFloat("Shell radius", &m_ShellRadius, 0.5f, 10.0f);

	// Max pen
	if (m_ProjectileActive && m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).GetPosition().x > maxX)
		maxX = m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).GetPosition().x;
	ImGui::Text("Max pen: %.3f", maxX);

	if (ImGui::Button("Reset"))
	{
		m_PhysicsSolver.ClearObjects();
		m_PhysicsSolver.ClearChainLinks();
		m_ProjectileActive = false;

		m_ArmorSize = glm::ivec2(20, 100);
		m_ArmorRadius = 0.5f;
		m_ArmorHardness = 0.5f;
		m_ArmorResistance = 1.0f;
		m_Slope = 0.0f;
	}

	// Performance
	ImGui::Separator();

	static bool gridEnabled = m_PhysicsSolver.GetGridStatus();
	ImGui::Checkbox("Enable Grid", &gridEnabled); m_PhysicsSolver.SetGridStatus(gridEnabled);
	ImGui::Checkbox("Debug Grid", &m_DebugGrid);
	ImGui::SliderInt("Substeps", &m_PhysicsSubsteps, 1, 16);

	ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}

void Armor::OnEvent(Eis::Event& e)
{
	m_CameraController.OnEvent(e);
}