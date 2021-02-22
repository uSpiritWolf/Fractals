#include "FractalsRender.h"

#include <Windows.h>
#include "GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Logger/Logger.h"

#include "Resources/fractalFrag.glsl.inl"
#include "Resources/fractalVert.glsl.inl"

#include "imgui.h"

float verticesOfTriangles[] = {
	 1.0f,  1.0f, 0.0f,  // top right
	 1.0f, -1.0f, 0.0f,  // bottom right
	-1.0f, -1.0f, 0.0f,  // bottom left
	-1.0f,  1.0f, 0.0f   // top left 
};
unsigned int indicesOfSquare[] = {
	0, 1, 3,   // first triangle of a square
	1, 2, 3    // second triangle of a square
};

math::vec2f	FractalsRender::s_defaultPosition = math::vec2f(-0.35209f, 0.09199f);
float		FractalsRender::s_defaultZoom = 1 / 0.00017f;

FractalsRender::FractalsRender()
	: m_EBO(0)
	, m_VAO(0)
	, m_VBO(0)
	, m_zoom(1.0f)
	, m_position(0.0f , 0.0f)
	, m_windowSize()
	, m_fractalsShader()
	, m_animationtime(0.0f)
	, m_enableAnimation(false)
	, m_samples(5)
	, m_threshold(4)
	, m_maxIterations(512)
{
	ResetLocation();
}

FractalsRender::~FractalsRender()
{
}

void FractalsRender::Init()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		const char* str = reinterpret_cast<const char*>(glewGetErrorString(err));
		Logger::Log(LogLevel::ERR, "glewInit failed: " + std::string(str));
	}

	m_fractalsShader.reset(new Shader());
	m_fractalsShader->Load(fractalVertexShader, fractalFragmentShader);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	//
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesOfTriangles), verticesOfTriangles, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesOfSquare), indicesOfSquare, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	//
}

void FractalsRender::OnUpdate(float dt)
{
	if (m_enableAnimation)
	{
		m_animationtime += dt;
	}

	ImGuiIO& io = ImGui::GetIO();

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 winSize = viewport->Size;

	{
		const float PAD = 10.0f;
		static int corner = 0;

		ImVec2 workPos = viewport->WorkPos;
		ImVec2 workSize = viewport->WorkSize;
		ImVec2 windowPos, window_pos_pivot;
		windowPos.x = (corner & 1) ? (workPos.x + workSize.x - PAD) : (workPos.x + PAD);
		windowPos.y = (corner & 2) ? (workPos.y + workSize.y - PAD) : (workPos.y + PAD);
		window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, window_pos_pivot);

		ImGuiWindowFlags overlay = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin("Info", nullptr, overlay))
		{
			ImGui::Text("Frametime %.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::Text("Animation Time: %.4f", m_animationtime);
			ImGui::End();
		}
	}

	if (ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::DragFloat("Zoom", &m_zoom, 2.f, 1.0f, 100000.0f);
		ImGui::DragFloat2("X/Y", m_position.raw, 0.00001f, -1.0f, 1.0f, "%.7f");
		ImGui::Separator();
		ImGui::Checkbox("Enable Animation", &m_enableAnimation);
		ImGui::PushItemWidth(150);
		ImGui::SameLine(160);
		if (ImGui::Button("Reset time"))
		{
			m_animationtime = 0.0f;
		}
		ImGui::Separator();
		if (ImGui::Button("Reset Location"))
		{
			ResetLocation();
		}
		ImGui::Separator();
		ImGui::SliderInt("Samples", &m_samples, 1, 15);
		ImGui::SliderInt("Threshold", &m_threshold, 1, 8);
		ImGui::SliderInt("Max Iterations", &m_maxIterations, 64, 4096);
		ImGui::End();
	}

	if(!io.WantCaptureMouse)
	{
		if (ImGui::IsMouseDragging(0))
		{
			ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
			math::vec2f windowScale(2.0f / winSize.y, 2.0f / winSize.y);
			m_offset = math::vec2f(dragDelta.x, -dragDelta.y) * (1.0f / m_zoom) * windowScale;
		}
		else
		{
			m_position += m_offset;
			m_offset = math::vec2f(.0f, .0f);
		}

		if (io.MouseWheel != 0.0f)
		{
			const float diff = m_zoom * 0.11f * io.MouseWheel; // add/subtract 11% of zoom
			m_zoom += diff;
			if (m_zoom < 1.0f)
			{
				m_zoom = 1.0f;
			}
		}
	}

	(*m_fractalsShader)["iResolution"] = m_windowSize;
	(*m_fractalsShader)["iScale"] = 1.0f / m_zoom;
	(*m_fractalsShader)["iPosition"] = m_position + m_offset;
	(*m_fractalsShader)["iTime"] = m_animationtime;

	(*m_fractalsShader)["iSamples"] = m_samples;
	(*m_fractalsShader)["iThreshold"] = m_threshold;
	(*m_fractalsShader)["iMaxIter"] = m_maxIterations;
}

void FractalsRender::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	m_fractalsShader->Bind();

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	m_fractalsShader->Unbind();
}

void FractalsRender::OnWindowSizeChanged(const math::vec2f& newSize)
{
	m_windowSize = newSize;
	if (m_fractalsShader)
	{
		(*m_fractalsShader)["iResolution"] = m_windowSize;
	}
}

void FractalsRender::ResetLocation()
{
	m_position = s_defaultPosition;
	m_zoom = s_defaultZoom;
}
