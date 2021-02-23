#pragma once

#include "Math/vec.h"

struct RenderConfig
{
	bool m_useCPU;

	float m_zoom;
	float m_threshold;
	int m_maxIterations;

	math::vec2f m_windowSize;
	math::vec2f m_position;
	math::vec2f m_offset;

	bool m_color;

	RenderConfig() : m_zoom(0), m_threshold(0), m_maxIterations(0), m_color(false), m_useCPU(false){}

	bool operator==(const RenderConfig& rhs) const
	{
		return m_zoom == rhs.m_zoom
			&& m_threshold == rhs.m_threshold
			&& m_maxIterations == rhs.m_maxIterations
			&& m_windowSize == rhs.m_windowSize
			&& m_position == rhs.m_position
			&& m_offset == rhs.m_offset;
	}

	bool operator!=(const RenderConfig& rhs) const
	{
		return !(*this == rhs);
	}
};
