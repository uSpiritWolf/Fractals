
#include "MandelbrotGPURender.h"
#include "MandelbrotGPURender.h"

#include "Data/RenderConfig.h"
#include "Shader.h"

#include "Resources/mandelbrot.glsl.inl"

float s_verticesOfTriangles[] = {
	 1.0f,  1.0f, 0.0f,  // top right
	 1.0f, -1.0f, 0.0f,  // bottom right
	-1.0f, -1.0f, 0.0f,  // bottom left
	-1.0f,  1.0f, 0.0f   // top left 
};
unsigned int s_indicesOfSquare[] = {
	0, 1, 3,   // first triangle of a square
	1, 2, 3    // second triangle of a square
};

MandelbrotGPURender::MandelbrotGPURender()
	: m_VAO(0)
	, m_VBO(0)
	, m_EBO(0)
{
}

MandelbrotGPURender::~MandelbrotGPURender()
{
}

void MandelbrotGPURender::Init()
{
	m_fractalsShader.reset(new Shader());
	m_fractalsShader->Load(fractalVertexShader, fractalFragmentShader);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	//
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_verticesOfTriangles), s_verticesOfTriangles, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_indicesOfSquare), s_indicesOfSquare, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	//
}

void MandelbrotGPURender::OnUpdate()
{
	if (m_fractalsShader->IsValid())
	{
		if (std::shared_ptr<RenderConfig> config = DataBinder<RenderConfig>::GetData())
		{
			(*m_fractalsShader)["iResolution"] = config->m_windowSize;
			(*m_fractalsShader)["iScale"] = 1.0f / config->m_zoom;
			(*m_fractalsShader)["iPosition"] = config->m_position + config->m_offset;
			(*m_fractalsShader)["iThreshold"] = config->m_threshold;
			(*m_fractalsShader)["iMaxIter"] = config->m_maxIterations;
			(*m_fractalsShader)["iColor"] = config->m_colorEnabled;
		}
	}
}

void MandelbrotGPURender::OnRender()
{
	if (m_fractalsShader->IsValid())
	{
		m_fractalsShader->Bind();

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		m_fractalsShader->Unbind();
	}
}
