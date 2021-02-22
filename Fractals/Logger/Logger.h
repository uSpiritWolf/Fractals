#pragma once

#include "ILogger.h"
#include <set>

class Logger
{
public:

	Logger();
	~Logger();

	static void MakeInstance();

	static void FreeInstance();

	static void AddLoger(ILogger* logger);

	static void Log(LogLevel level, const std::string& msg);

private:
	std::set<ILogger*> m_loggers;

	static Logger* s_instance;
};