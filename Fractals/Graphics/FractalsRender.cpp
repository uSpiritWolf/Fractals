#include "FractalsRender.h"

#include "Logger/Logger.h"

#include "GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui.h"
#include "UI/ToolsUI.h"

#include "Data/RenderConfig.h"

#include "Resources/mandelbrot.glsl.inl"

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

FractalsRender::FractalsRender()
	: m_EBO(0)
	, m_VAO(0)
	, m_VBO(0)
	, m_windowSize()
	, m_fractalsShader()
	, m_mandelbrotConfig(new RenderConfig())
	, m_toolsUI(new ToolsUI())
{
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

	m_toolsUI->BindConfig(m_mandelbrotConfig);
}

void FractalsRender::OnUpdate(float dt)
{
	UpdateGUI();
	UpdateInput();
	SetupShaderValue();
}

void FractalsRender::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (m_fractalsShader->IsValid())
	{
		m_fractalsShader->Bind();

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		m_fractalsShader->Unbind();
	}
}

void FractalsRender::OnWindowSizeChanged(const math::vec2f& newSize)
{
	m_windowSize = newSize;
}

void FractalsRender::UpdateGUI()
{
	m_toolsUI->Update();
}

void FractalsRender::UpdateInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (!io.WantCaptureMouse)
	{
		if (ImGui::IsMouseDragging(0))
		{
			const ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
			const math::vec2f windowScale(2.f / m_windowSize.y, 2.f / m_windowSize.y);
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

void FractalsRender::SetupShaderValue()
{
	if (m_fractalsShader->IsValid())
	{
		(*m_fractalsShader)["iResolution"] = m_windowSize;
		(*m_fractalsShader)["iScale"] = 1.0f / m_mandelbrotConfig->m_zoom;
		(*m_fractalsShader)["iPosition"] = m_mandelbrotConfig->m_position + m_mandelbrotConfig->m_offset;
		(*m_fractalsShader)["iThreshold"] = m_mandelbrotConfig->m_threshold;
		(*m_fractalsShader)["iMaxIter"] = m_mandelbrotConfig->m_maxIterations;
	}
}
