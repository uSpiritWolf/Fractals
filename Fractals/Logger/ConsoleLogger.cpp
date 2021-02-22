#include "ConsoleLogger.h"
#include <chrono>
#include <ctime>  

ConsoleLogger::ConsoleLogger()
	: m_logLevel(LogLevel::INFO)
{
}

ConsoleLogger::~ConsoleLogger()
{
}

void ConsoleLogger::Log(LogLevel level, const std::string& msg)
{
	if (level > m_logLevel)
		return;

	const std::time_t now = std::time(nullptr);
	char buf[24] = {};
	if (strftime(buf, sizeof buf, "[%H:%M:%S]", std::localtime(&now))) 
	{
		std::cout << buf;
	}

	switch (level)
	{
	case LogLevel::DEBUG:
		std::cout << "[DEBUG]";
		break;
	case LogLevel::ERR:
		std::cerr << "[ERROR]";
		break;
	case LogLevel::INFO:
		std::cout << "[INFO]";
		break;
	}

	std::cout << ": " << msg << std::endl;
}

void ConsoleLogger::SetLevel(LogLevel level)
{
	m_logLevel = level;
}
