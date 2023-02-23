#include "Test.h"

Test::Test() : Layer("PhysicsTest"), m_CameraController(16.0f / 9.0f), m_PreviewPos(0.0f), m_PreviewDiameter(1.0f), m_Flood(false)
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
	if (m_Flood && m_PhysicsSolver.GetObjectPool().size() < 1350)
	{
		VerletObject obj(m_PreviewPos, m_PreviewDiameter, glm::vec2(Eis::Random::Float(0.0f, 1.0f), Eis::Random::Float(0.0f, 1.0f)));
		obj.SetColor({ Eis::Random::Float(0.0f, 1.0f), Eis::Random::Float(0.0f, 1.0f), Eis::Random::Float(0.0f, 1.0f) }); // TODO: hack - .back won't return reference
		m_PhysicsSolver.AddObject(obj);
	}

	Interactions();
	m_PhysicsSolver.Update(ts);
	RenderPhysicsObjects();

	// Spawn preview
	Eis::Renderer2D::DrawQuad(glm::vec3(m_PreviewPos, 1.0f), glm::vec2(m_PreviewDiameter), m_CircleTexture, { 1.0f, 1.0f, 1.0f, 0.3f });

	Eis::Renderer2D::EndScene();

	FPSLimiter();
}

void Test::OnImGuiRender()
{
	ImGui::Begin("Simulation");

	// Object count
	ImGui::Text("Object count: %.0i", m_PhysicsSolver.GetObjectPool().size());
	if (ImGui::Button("Clear Objects"))
		m_PhysicsSolver.ClearObjects();

	if (ImGui::Button("Reset Movement"))
		for (VerletObject& obj : m_PhysicsSolver.GetObjectPool())
			obj.StopMovement();

	// Set scene diameter
	static float tmp = m_PhysicsSolver.GetConstraintRadius();
	ImGui::Text("Constraint diameter:");
	ImGui::SliderFloat("", &tmp, 0.001f, 10.0f);
	m_PhysicsSolver.SetConstraintRadius(tmp / 2.0f);

	ImGui::Separator();

	// Spawn new object
	ImGui::SliderFloat2("Spawn pos", (float*)&m_PreviewPos, -m_PhysicsSolver.GetConstraintRadius(), m_PhysicsSolver.GetConstraintRadius());
	ImGui::SliderFloat("Diameter", &m_PreviewDiameter, 0.1f, 10.0f);
	if (ImGui::Button("Safe Spawn") && !m_PhysicsSolver.CheckCollision(m_PreviewPos, m_PreviewDiameter / 2.0f))
		m_PhysicsSolver.AddObject(m_PreviewPos, m_PreviewDiameter);
	if (ImGui::Button("Force Spawn"))
		m_PhysicsSolver.AddObject(m_PreviewPos, m_PreviewDiameter);

	ImGui::Checkbox("Flood", &m_Flood);

	// Debug info
	ImGui::Separator();

	ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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

void Test::RenderPhysicsObjects() const
{
	for (VerletObject& obj : m_PhysicsSolver.GetObjectPool())
		Eis::Renderer2D::DrawQuad(glm::vec3(obj.GetPosition(), 1.0f), glm::vec2(obj.GetDiameter()), m_CircleTexture, glm::vec4(obj.GetColor(), 1.0f));
}

// HACK: fps limiter
void Test::FPSLimiter()
{
	static std::chrono::time_point last = std::chrono::high_resolution_clock::now();

	while (std::chrono::high_resolution_clock::now() - last < std::chrono::milliseconds(11))
		Sleep(1);

	last = std::chrono::high_resolution_clock::now();
}