#pragma once

#include <memory>

#include "Shader.h"
#include "Math/vec.h"

class ToolsUI;
struct RenderConfig;

class FractalsRender
{
public:

	FractalsRender();
	~FractalsRender();

	void Init();

	void OnUpdate(float dt);
	void OnRender();

	void OnWindowSizeChanged(const math::vec2f& newSize);

private:
	void UpdateGUI();
	void UpdateInput();
	void SetupShaderValue();

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	math::vec2f m_windowSize;

	std::unique_ptr<Shader> m_fractalsShader;

	std::shared_ptr<RenderConfig> m_mandelbrotConfig;

	std::unique_ptr<ToolsUI> m_toolsUI;
};