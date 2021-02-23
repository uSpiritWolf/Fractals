#pragma once

#include <xstring>
#include <functional>
#include "Math/vec.h"

class IWindow
{
public:

	virtual ~IWindow() {}

	enum class ErrorCode
	{
		NONE,
		ERR_SYSCALL,
		ERR_GL,
		ERR_SWAPBUFFER,
		ERR_GLEW
	};

	virtual void Init() = 0;
	virtual void Destroy() = 0;
	virtual bool HasError() const = 0;
	virtual ErrorCode GetError() const = 0;
	virtual const std::string& GetErrorString() const = 0;

	virtual void SetWindowSizeChanged(const std::function<void(const math::vec2f& newSize)>& callback) = 0;

	virtual bool IsValid() const = 0;

	virtual void SwapBuffer() = 0;
};