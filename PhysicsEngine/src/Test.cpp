#include "Test.h"

Test::Test() : Layer("PhysicsTest"), m_CameraController(16.0f / 9.0f), m_PreviewPos(0.0f), m_PreviewDiameter(1.0f), m_Flood(false), m_Img("assets/textures/nazi.png"), m_ColorMem()
{}

void Test::OnAttach()
{
	EIS_INFO("Loading assets...");
	m_CircleTexture = Eis::Texture2D::Create("assets/textures/circle.png");
	EIS_INFO("Loaded assets!");

	m_CameraController.OnEvent(Eis::MouseScrolledEvent(0.0f, -90.0f)); // HACK: artificial camera zoom

	m_PhysicsSolver.SetConstraintRadius(25.0f);
}

void Test::OnDetach()
{
}

void Test::OnUpdate(Eis::TimeStep ts)
{
	m_CameraController.OnUpdate(ts);

	Eis::RenderCommands::SetClearColor({ 0.3f, 0.3f, 0.3f, 1.0f });
	Eis::RenderCommands::Clear();

	Eis::Renderer2D::BeginScene(m_CameraController.GetCamera());

	// Background
	Eis::Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(m_PhysicsSolver.GetConstraintRadius() * 2.0f), m_CircleTexture, {0.12f, 0.12f, 0.12f, 1.0f});

	// Flood
	if (m_Flood)
	{
		m_PhysicsSolver.AddObject(m_PreviewPos, m_PreviewDiameter, glm::vec3(FLT_EPSILON));
		m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).SetColor({Eis::Random::Float(0.0f, 1.0f), Eis::Random::Float(0.0f, 1.0f), Eis::Random::Float(0.0f, 1.0f)});
	}

	Interactions();
	m_PhysicsSolver.Update(Eis::TimeStep(0.0136f)); // fully deterministic engine
	RenderPhysicsObjects();

	// Spawn preview
	Eis::Renderer2D::DrawQuad(glm::vec3(m_PreviewPos, 1.0f), glm::vec2(m_PreviewDiameter), m_CircleTexture, { 1.0f, 1.0f, 1.0f, 0.3f });

	Eis::Renderer2D::EndScene();
}

void Test::OnImGuiRender()
{
	ImGui::Begin("Simulation");

	// Object count
	ImGui::Text("Object count: %.0i", m_PhysicsSolver.GetObjectPool().size());
	if (ImGui::Button("Clear Objects"))
	{
		m_PhysicsSolver.ClearObjects();
		m_PhysicsSolver.ClearChainLinks();
	}

	if (ImGui::Button("Reset Movement"))
		for (int i = 0; i < m_PhysicsSolver.GetObjectPool().size(); i++)
			m_PhysicsSolver.GetObjectRef(i).StopMovement();

	// Set scene diameter
	static float tmp = m_PhysicsSolver.GetConstraintRadius();
	ImGui::Text("Constraint diameter:");
	ImGui::SliderFloat("", &tmp, 0.001f, 30.0f);
	m_PhysicsSolver.SetConstraintRadius(tmp / 2.0f);

	ImGui::Separator();

	// Spawn new object
	ImGui::SliderFloat2("Spawn pos", (float*)&m_PreviewPos, -m_PhysicsSolver.GetConstraintRadius(), m_PhysicsSolver.GetConstraintRadius());
	ImGui::SliderFloat("Diameter", &m_PreviewDiameter, 0.1f, 10.0f);
	if (ImGui::Button("Safe Spawn") && !m_PhysicsSolver.CheckCollision(m_PreviewPos, m_PreviewDiameter))
		m_PhysicsSolver.AddObject(m_PreviewPos, m_PreviewDiameter);
	if (ImGui::Button("Force Spawn"))
		m_PhysicsSolver.AddObject(m_PreviewPos, m_PreviewDiameter);

	ImGui::Checkbox("Flood", &m_Flood);

	// Spawn chain
	if (ImGui::Button("Spawn chain"))
	{
		int lastObjId = m_PhysicsSolver.GetObjectPool().size();
		for (int i = 0; i < 20; i++)
			m_PhysicsSolver.AddObject({ i - 10.0f, 0.0f }, 1.0f);

		for (int i = lastObjId + 1; i < lastObjId + 20; i++)
			m_PhysicsSolver.AddChainLink(i - 1, i);

		m_PhysicsSolver.GetObjectRef(lastObjId).Lock();
		m_PhysicsSolver.GetObjectRef(lastObjId + 19).Lock();
	}

	// Performance
	ImGui::Separator();

	static int substeps = m_PhysicsSolver.GetSubsteps();
	ImGui::SliderInt("Substeps", &substeps, 1, 16);
	m_PhysicsSolver.SetSubsteps(substeps);

	ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();

	//--------------------------------------------------------------------------------

	ImGui::Begin("Image Generator");

	static bool spawn = false;
	ImGui::Checkbox("Spawn", &spawn);
	if (spawn && m_PhysicsSolver.GetObjectPool().size() < 500 && !m_PhysicsSolver.CheckCollision({ 0.0f, 12.0f }, 1.0f))
	{
		m_PhysicsSolver.AddObject({0.0f, 12.0f}, 1.0f, glm::vec2(FLT_EPSILON, -25000.0f));
		m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).SetColor(m_ColorMem[m_PhysicsSolver.GetObjectPool().size() - 1]);
	}

	if (ImGui::Button("Color"))
	{
		for (int i = 0; i < m_PhysicsSolver.GetObjectPool().size(); i++)
		{
			int x = (int)m_PhysicsSolver.GetObjectRef(i).GetPosition().x + 12;
			int y = (int)m_PhysicsSolver.GetObjectRef(i).GetPosition().y + 12;

			m_PhysicsSolver.GetObjectRef(i).SetColor(m_Img.GetPixel(x, y) / 255.0f);
			m_ColorMem[i] = m_PhysicsSolver.GetObjectRef(i).GetColor();

			glm::vec3 color = m_ColorMem[i];
		}
	}

	ImGui::End();
}

void Test::OnEvent(Eis::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Test::Interactions()
{
	static uint8_t safeguard = 0b1;
	if (Eis::Input::IsKeyPressed(EIS_KEY_E))
	{
		if (safeguard & 0b1)
		{
			m_PhysicsSolver.AddObject(glm::vec2(Eis::Input::GetMousePos().first, Eis::Input::GetMousePos().second));
			safeguard ^= 0b1;
		}
	}
	else
		safeguard |= 0b1;
}

// Eis cant draw pure circles (yet)
void Test::RenderPhysicsObjects() const
{
	for (VerletObject& obj : m_PhysicsSolver.GetObjectPool())
		Eis::Renderer2D::DrawQuad(glm::vec3(obj.GetPosition(), 1.0f), glm::vec2(obj.GetDiameter()), m_CircleTexture, glm::vec4(obj.GetColor(), 1.0f));
}