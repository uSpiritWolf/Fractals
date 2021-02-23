#pragma once

#include <memory>

class IApp;
class FractalsRender;

class FractalsApplication
{
public:

	FractalsApplication();

	~FractalsApplication();

	void Init();

	int Run();

private:

	void OnUpdate(float dt);
	void OnRender();

	std::unique_ptr<IApp> m_app;
	std::unique_ptr<FractalsRender> m_fractalRender;
};