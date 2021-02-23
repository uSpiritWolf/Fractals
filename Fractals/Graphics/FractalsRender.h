#pragma once

#include "Data/DataProvider.h"
#include "Math/vec.h"

class MandelbrotGPURender;
class MandelbrotCPURender;
class ToolsUI;
struct RenderConfig;

class FractalsRender : public DataProvider<RenderConfig>
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
	std::unique_ptr<MandelbrotCPURender> m_mandelbrotCPURender;

	std::unique_ptr<ToolsUI> m_toolsUI;
};