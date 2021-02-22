#pragma once

#include <iostream>

enum class LogLevel
{
	ERR,
	DEBUG,
	INFO,
};

class ILogger
{
public:
	virtual ~ILogger() {};

	virtual void Log(LogLevel level, const std::string& msg) = 0;
	virtual void SetLevel(LogLevel) = 0;
};