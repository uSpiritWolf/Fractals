#include "ToolsUI.h"

#include "imgui.h"

math::vec2f	ToolsUI::s_defaultPosition = math::vec2f(0.0f, 0.0f);
float		ToolsUI::s_defaultZoom = 1;
int			ToolsUI::s_defaultMaxIter = 512;
float		ToolsUI::s_defaultThreshold = 65535;

ToolsUI::ToolsUI()
{
}

ToolsUI::~ToolsUI()
{
}

void ToolsUI::BindConfig(const std::weak_ptr<RenderConfig>& config)
{
	m_config = config;
	Reset();
}

void ToolsUI::Update()
{
	const float PAD = 10.0f;
	static int corner = 0;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 workPos = viewport->WorkPos;
	const ImVec2 workSize = viewport->WorkSize;
	ImVec2 windowPos, window_pos_pivot;
	windowPos.x = (corner & 1) ? (workPos.x + workSize.x - PAD) : (workPos.x + PAD);
	windowPos.y = (corner & 2) ? (workPos.y + workSize.y - PAD) : (workPos.y + PAD);
	window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
	window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, window_pos_pivot);

	const ImGuiWindowFlags overlay = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowBgAlpha(0.35f);
	if (ImGui::Begin("Info", nullptr, overlay))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Frametime %.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	if (std::shared_ptr<RenderConfig> config = m_config.lock())
	{
		if (ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::DragFloat("Zoom", &config->m_zoom, 2.f, 1.0f, 100000.0f);
			ImGui::DragFloat2("X/Y", config->m_position.raw, 0.00001f, -1.0f, 1.0f, "%.7f");
			ImGui::Separator();
			ImGui::SliderInt("Max Iterations", &config->m_maxIterations, 64, 4096);
			ImGui::InputFloat("Threshold", &config->m_threshold);
			ImGui::Separator();
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
			ImGui::End();
		}
	}
}

void ToolsUI::Reset()
{
	if (std::shared_ptr<RenderConfig> config = m_config.lock())
	{
		config->m_position = s_defaultPosition;
		config->m_zoom = s_defaultZoom;
		config->m_maxIterations = s_defaultMaxIter;
		config->m_threshold = s_defaultThreshold;
	}
}

