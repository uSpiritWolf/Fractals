#pragma once

#include <memory>
#include "GL/glew.h"

class Shader;
struct RenderConfig;

class MandelbrotGPURender
{
public:
	MandelbrotGPURender();
	~MandelbrotGPURender();

	void Init();

	void OnUpdate();
	void OnRender();

	void BindConfig(const std::weak_ptr<RenderConfig>& config);
	void ResetBind();

private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	std::unique_ptr<Shader> m_fractalsShader;
	std::weak_ptr<RenderConfig> m_config;
};