#pragma once

#include "Data/DataBinder.h"
#include "GL/glew.h"

class Shader;
struct RenderConfig;

class MandelbrotGPURender : public DataBinder<RenderConfig>
{
public:
	MandelbrotGPURender();
	~MandelbrotGPURender();

	void Init();

	void OnUpdate();
	void OnRender();

private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	std::unique_ptr<Shader> m_fractalsShader;
};