#pragma once

#include "Math/vec.h"

struct RenderConfig
{
	float m_zoom;
	float m_threshold;
	int m_maxIterations;

	math::vec2f m_position;
	math::vec2f m_offset;

	RenderConfig() : m_zoom(0), m_threshold(0), m_maxIterations(0) {}
};
