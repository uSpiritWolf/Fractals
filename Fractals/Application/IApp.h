#pragma once

#include <functional>
#include "Math/vec.h"

class IApp 
{
public:
	virtual ~IApp() {}

	virtual int Run() = 0;

	virtual void Init() = 0;

	virtual void SetUpdateCallback(const std::function<void(float dt)>& callback) = 0;
	virtual void SetRenderCallback(const std::function<void()>& callback) = 0;
	virtual void SetWindowSizeChanged(const std::function<void(const math::vec2f& newSize)>& callback) = 0;
};