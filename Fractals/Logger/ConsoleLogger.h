#pragma once

#include "ILogger.h"

class ConsoleLogger : public ILogger
{
public:

	ConsoleLogger();
	virtual ~ConsoleLogger();

	void Log(LogLevel level, const std::string& msg) override;

	void SetLevel(LogLevel level) override;

private:
	LogLevel m_logLevel;
};