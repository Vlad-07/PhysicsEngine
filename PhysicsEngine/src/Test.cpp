#include "Test.h"

Test::Test()
	: Layer("PhysicsTest"), m_CameraController(16.0f / 9.0f), m_PreviewPos(0.0f),
	  m_PreviewDiameter(1.0f), m_PhysicsSubsteps(8), m_Flood(false), m_Img("assets/textures/cat.png"), m_ColorMem(), m_DebugGrid(false), m_ThreadCount(1)
{}

void Test::OnAttach()
{
	EIS_INFO("Loading assets...");
	m_CircleTexture = Eis::Texture2D::Create("assets/textures/circle.png");
	EIS_INFO("Loaded assets!");

	m_CameraController.OnEvent(Eis::MouseScrolledEvent(0.0f, -130.0f)); // HACK: artificial camera zoom
}

void Test::OnDetach()
{
}

void Test::OnUpdate(Eis::TimeStep ts)
{
	m_CameraController.OnUpdate(ts);

	Eis::RenderCommands::SetClearColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	Eis::RenderCommands::Clear();

	Eis::Renderer2D::BeginScene(m_CameraController.GetCamera());

	// Background
	if (!m_DebugGrid) Eis::Renderer2D::DrawQuad(glm::vec2(0.0f), m_PhysicsSolver.GetConstraintDimensions() * 2.0f, glm::vec4(0.06f, 0.06f, 0.06f, 0.5f));

	// Flood
	if (m_Flood)
	{
		m_PhysicsSolver.AddObject(m_PreviewPos - glm::vec2(m_PreviewDiameter / 2.0f - 0.1f, -m_PreviewDiameter / 2.0f - 0.1f), m_PreviewDiameter, glm::vec2(FLT_EPSILON));
		m_PhysicsSolver.AddObject(m_PreviewPos + glm::vec2(m_PreviewDiameter / 2.0f - 0.1f,  m_PreviewDiameter / 2.0f - 0.1f), m_PreviewDiameter, glm::vec2(FLT_EPSILON));
																																						  
		m_PhysicsSolver.AddObject(m_PreviewPos - glm::vec2(m_PreviewDiameter / 2.0f - 0.1f,  m_PreviewDiameter / 2.0f - 0.1f), m_PreviewDiameter, glm::vec2(FLT_EPSILON));
		m_PhysicsSolver.AddObject(m_PreviewPos + glm::vec2(m_PreviewDiameter / 2.0f - 0.1f, -m_PreviewDiameter / 2.0f - 0.1f), m_PreviewDiameter, glm::vec2(FLT_EPSILON));
	}

	m_PhysicsSolver.UpdatePhysics(Eis::TimeStep(0.0136f), m_PhysicsSubsteps); // fully deterministic engine

	if (m_DebugGrid) DebugGrid();

	RenderPhysicsObjects();

	// Spawn preview
	Eis::Renderer2D::DrawQuad(glm::vec3(m_PreviewPos, 1.0f), glm::vec2(m_PreviewDiameter), m_CircleTexture, glm::vec4(1.0f, 1.0f, 1.0f, 0.3f));

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

	// Set scene dimensions
	static glm::vec2 tmp = m_PhysicsSolver.GetConstraintDimensions();
	ImGui::Text("Constraint dimensions:");
	ImGui::SliderFloat2("", &tmp.x, 0.001f, 60.0f);
	m_PhysicsSolver.SetConstraintDimensions(tmp);

	ImGui::Separator();

	// Spawn new object
	float maxDim = std::max(m_PhysicsSolver.GetConstraintDimensions().x, m_PhysicsSolver.GetConstraintDimensions().y);
	ImGui::SliderFloat2("Spawn pos", (float*)&m_PreviewPos, -maxDim, maxDim); // TODO: x-y specific limits
	ImGui::SliderFloat("Diameter", &m_PreviewDiameter, 0.1f, 10.0f);

	if (ImGui::Button("Safe Spawn") && !m_PhysicsSolver.CheckCollision(m_PreviewPos, m_PreviewDiameter / 2))
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

	static bool gridEnabled = m_PhysicsSolver.GetGridStatus();
	ImGui::Checkbox("Enable Grid", &gridEnabled); m_PhysicsSolver.SetGridStatus(gridEnabled);
	ImGui::Checkbox("Debug Grid", &m_DebugGrid);
	ImGui::BeginDisabled();
	ImGui::SliderInt("Thread Count", &m_ThreadCount, 1, 16);
	ImGui::EndDisabled();
	ImGui::SliderInt("Substeps", &m_PhysicsSubsteps, 1, 16);

	ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();

	//-----------------------------------------------------------------------------------------------------

	ImGui::Begin("Image Generator");

	static int maxObjects = 500;
	static float objDiameter = 1.0f;
	static int quality = 2; int q = quality;
	ImGui::SliderInt("Quality", &quality, 1, 3);
	quality = m_PhysicsSolver.GetObjectPool().size() ? q : quality;

	switch (quality)
	{
	case 1:
		maxObjects = 1300;
		objDiameter = 1.5f;
		break;

	case 2:
		maxObjects = 2800;
		objDiameter = 1.0f;
		break;

	case 3:
		maxObjects = 11500;
		objDiameter = 0.5f;
		break;

	default:
		EIS_ERROR("Invalid image quality selected!");
	}

	static bool spawn = false;
	ImGui::Checkbox("Spawn", &spawn);
	if (spawn && m_PhysicsSolver.GetObjectPool().size() < maxObjects)
	{
		const float ySpawnPos = 0.9f;
		for (int i = 0; i <= m_PhysicsSolver.GetObjectPool().size() / 100 && m_PhysicsSolver.GetObjectPool().size() < maxObjects && i < 10; i++)
		{
			if (!m_PhysicsSolver.CheckCollision({ -m_PhysicsSolver.GetConstraintDimensions().x + objDiameter, m_PhysicsSolver.GetConstraintDimensions().y * ySpawnPos - i * objDiameter * 1.5f }, objDiameter))
			{
				m_PhysicsSolver.AddObject({ -m_PhysicsSolver.GetConstraintDimensions().x + objDiameter, m_PhysicsSolver.GetConstraintDimensions().y * ySpawnPos - i * objDiameter * 1.5f }, objDiameter, glm::vec2(50000.0f, 0.0f));
				m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).SetColor(m_ColorMem[m_PhysicsSolver.GetObjectPool().size() - 1]);
			}
		}
	}

	if (ImGui::Button("Color"))
	{
		for (int i = 0; i < m_PhysicsSolver.GetObjectPool().size(); i++)
		{
			// Turn world space coords to a [0, 1] range
			float x = (m_PhysicsSolver.GetObjectRef(i).GetPosition().x / m_PhysicsSolver.GetConstraintDimensions().x + 1.0f) / 2.0f;
			float y = (m_PhysicsSolver.GetObjectRef(i).GetPosition().y / m_PhysicsSolver.GetConstraintDimensions().y + 1.0f) / 2.0f;

			// And multiply by the image size to get pixel coords
			x *= m_Img.GetWidth();
			y *= m_Img.GetHeight();

			// More objects than the colorMem size can trigger an exception in GetPixel
			m_PhysicsSolver.GetObjectRef(i).SetColor(m_Img.GetPixel((int)x, (int)y) / 255.0f);
			m_ColorMem[i] = m_PhysicsSolver.GetObjectRef(i).GetColor();

			glm::vec3 color = m_ColorMem[i];
		}
	}

	ImGui::End();

	//-----------------------------------------------------------------------------------------------------

	ImGui::Begin("Double Pendulum");

	static bool collisions = true;
	ImGui::Checkbox("Collisions", &collisions);
	m_PhysicsSolver.SetCollisionDetection(collisions);

	if (ImGui::Button("Spawn pendulum"))
	{
		m_PhysicsSolver.AddObject(glm::vec2(0.0f));
		m_PhysicsSolver.GetObjectRef(m_PhysicsSolver.GetObjectPool().size() - 1).Lock();

		m_PhysicsSolver.AddObject(glm::vec2(0.0f, -5.0f));
		m_PhysicsSolver.AddChainLink(m_PhysicsSolver.GetObjectPool().size() - 2, m_PhysicsSolver.GetObjectPool().size() - 1, 5.0f);

		m_PhysicsSolver.AddObject(glm::vec2(0.001f));
		m_PhysicsSolver.AddChainLink(m_PhysicsSolver.GetObjectPool().size() - 2, m_PhysicsSolver.GetObjectPool().size() - 1, 5.0f);
	}

	ImGui::End();
}

void Test::OnEvent(Eis::Event& e)
{
	m_CameraController.OnEvent(e);
}

// Eis cant draw pure circles (yet)
void Test::RenderPhysicsObjects() const
{
	for (VerletObject& obj : m_PhysicsSolver.GetObjectPool())
		Eis::Renderer2D::DrawQuad(glm::vec3(obj.GetPosition(), 1.0f), glm::vec2(obj.GetDiameter()), m_CircleTexture, glm::vec4(obj.GetColor(), 1.0f));
}

void Test::DebugGrid()
{
	for (int y = 0; y < m_PhysicsSolver.GetGrid().GetHeight(); y++)
	{
		for (int x = 0; x < m_PhysicsSolver.GetGrid().GetWidth(); x++)
		{
			glm::vec4 color;
			if (m_PhysicsSolver.GetGrid()[y][x].objects.size())
				color = glm::vec4(0.2f, 0.8f, 0.2f, 0.8f);
			else
				color = glm::vec4(0.2f, 0.2f, 0.2f, 0.8f);

			Eis::Renderer2D::DrawQuad({ x * 2 - m_PhysicsSolver.GetConstraintDimensions().x + 1.0f, y * 2 - m_PhysicsSolver.GetConstraintDimensions().y + 1.0f }, glm::vec2(2.0f), color);
		}
	}
}