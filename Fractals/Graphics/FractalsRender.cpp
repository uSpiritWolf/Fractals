#include "FractalsRender.h"
#include "MandelbrotGPURender.h"

#include "Logger/Logger.h"

#include "imgui.h"
#include "UI/ToolsUI.h"

#include "Data/RenderConfig.h"

FractalsRender::FractalsRender()
	: DataProvider<RenderConfig>(m_mandelbrotConfig)
	, m_mandelbrotConfig(new RenderConfig())
	, m_mandelbrotGPURender(new MandelbrotGPURender())
	, m_toolsUI(new ToolsUI())
{
}

FractalsRender::~FractalsRender()
{
	m_toolsUI->ResetBind();
	m_mandelbrotGPURender->ResetBind();
}

void FractalsRender::Init()
{
	ProvideData(*m_toolsUI);
	ProvideData(*m_mandelbrotGPURender);
	m_toolsUI->Reset();
	m_mandelbrotGPURender->Init();
}

void FractalsRender::OnUpdate(float dt)
{
	UpdateGUI();
	UpdateInput();
}

void FractalsRender::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	m_mandelbrotGPURender->OnRender();
}

void FractalsRender::OnWindowSizeChanged(const math::vec2f& newSize)
{
	m_mandelbrotConfig->m_windowSize = newSize;
}

void FractalsRender::UpdateGUI()
{
	m_toolsUI->Update();
	m_mandelbrotGPURender->OnUpdate();
}

void FractalsRender::UpdateInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (!io.WantCaptureMouse)
	{
		if (ImGui::IsMouseDragging(0))
		{
			const ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
			const math::vec2f windowScale(2.f / m_mandelbrotConfig->m_windowSize.y, 2.f / m_mandelbrotConfig->m_windowSize.y);
			m_mandelbrotConfig->m_offset = math::vec2f(dragDelta.x, -dragDelta.y) * (1.0f / m_mandelbrotConfig->m_zoom) * windowScale;
		}
		else
		{
			m_mandelbrotConfig->m_position += m_mandelbrotConfig->m_offset;
			m_mandelbrotConfig->m_offset = math::vec2f(.0f, .0f);
		}

		if (io.MouseWheel != 0.0f)
		{
			const float diff = m_mandelbrotConfig->m_zoom * 0.11f * io.MouseWheel; // add/subtract 11% of zoom
			m_mandelbrotConfig->m_zoom += diff;
			if (m_mandelbrotConfig->m_zoom < 1.0f)
			{
				m_mandelbrotConfig->m_zoom = 1.0f;
			}
		}
	}
}
