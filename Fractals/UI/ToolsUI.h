#pragma once

#include <memory>
#include "Math/vec.h"

struct RenderConfig;

class ToolsUI
{
public:
	ToolsUI();
	~ToolsUI();

	void BindConfig(const std::weak_ptr<RenderConfig>& config);
	void ResetBind();

	void Update();
private:
	void Reset();

	std::weak_ptr<RenderConfig>	m_config;

	static math::vec2f	s_defaultPosition;
	static float		s_defaultZoom;
	static int			s_defaultMaxIter;
	static float		s_defaultThreshold;
	static bool			s_defaultColor;
};