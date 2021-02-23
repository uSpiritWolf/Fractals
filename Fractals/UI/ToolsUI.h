#pragma once

#include "Data/DataBinder.h"
#include "Math/vec.h"

struct RenderConfig;

class ToolsUI : public DataBinder<RenderConfig>
{
public:
	ToolsUI();
	~ToolsUI();

	void Update();
	void Reset();

private:

	static math::vec2d	s_defaultPosition;
	static float		s_defaultZoom;
	static int			s_defaultMaxIter;
	static float		s_defaultThreshold;
	static bool			s_defaultColor;
	static bool			s_defaultUseCPU;
};