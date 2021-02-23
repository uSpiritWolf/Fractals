#include "Logger/Logger.h"
#include "Logger/ConsoleLogger.h"

#include "Application/FractalsApplication.h"

#include "Math/vec.h"

int main()
{
	Logger::MakeInstance();
	Logger::AddLoger(new ConsoleLogger());

	FractalsApplication app;
	app.Init();

	const int result = app.Run();

	Logger::FreeInstance();

	return result;
}
