#include "Logger.h"

Logger* Logger::s_instance = nullptr;

Logger::Logger()
{
}

Logger::~Logger()
{
	for (ILogger* log : m_loggers)
	{
		delete log;
	}
}

void Logger::MakeInstance()
{
	s_instance = new Logger();
}

void Logger::FreeInstance()
{
	delete s_instance;
}

void Logger::AddLoger(ILogger* logger)
{
	if (s_instance && logger)
	{
		s_instance->m_loggers.insert(logger);
	}
}

void Logger::Log(LogLevel level, const std::string& msg)
{
	if (s_instance)
	{
		for (ILogger* log : s_instance->m_loggers)
		{
			log->Log(level, msg);
		}
	}
}
