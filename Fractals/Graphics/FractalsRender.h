#pragma once

#include <memory>

#include "Math/vec.h"

class MandelbrotGPURender;
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

	std::shared_ptr<RenderConfig> m_mandelbrotConfig;
	std::unique_ptr<MandelbrotGPURender> m_mandelbrotGPURender;

	std::unique_ptr<ToolsUI> m_toolsUI;
};