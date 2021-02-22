#pragma once

#include "Shader.h"
#include "Math/vec.h"
#include <memory>

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

	void ResetLocation();

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	float m_zoom;
	math::vec2f m_position;
	math::vec2f m_offset;
	math::vec2f m_windowSize;
	float m_animationtime;

	int m_samples;
	int m_threshold;
	int m_maxIterations;

	bool m_enableAnimation;

	std::unique_ptr<Shader> m_fractalsShader;

	static math::vec2f	s_defaultPosition;
	static float		s_defaultZoom;
};