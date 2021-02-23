#include "FractalsApplication.h"

#include "Logger/Logger.h"

#if defined(WIN32)
#include "Application/Win32App.h"
#endif

#include "Graphics/FractalsRender.h"

FractalsApplication::FractalsApplication()
	: m_app(new Win32App())
	, m_fractalRender(new FractalsRender())
{
}

FractalsApplication::~FractalsApplication()
{
}

void FractalsApplication::Init()
{
	Logger::Log(LogLevel::INFO, "Initialization");

	if (m_app == nullptr)
	{
		Logger::Log(LogLevel::ERR, "Application pointer is null");
		return;
	}

	if (m_fractalRender == nullptr)
	{
		Logger::Log(LogLevel::ERR, "FractalsRender pointer is null");
		return;
	}

	m_app->SetUpdateCallback(std::bind(&FractalsApplication::OnUpdate, this, std::placeholders::_1));
	m_app->SetRenderCallback(std::bind(&FractalsApplication::OnRender, this));
	m_app->SetWindowSizeChanged([this](const math::vec2f& newSize)
	{
		m_fractalRender->OnWindowSizeChanged(newSize);
	});

	m_app->Init();
	m_fractalRender->Init();
	Logger::Log(LogLevel::INFO, "Initialization finished");
}

int FractalsApplication::Run()
{
	if (m_app == nullptr)
	{
		return -1;
	}
	return m_app->Run();
}

void FractalsApplication::OnUpdate(float dt)
{
	m_fractalRender->OnUpdate(dt);
}

void FractalsApplication::OnRender()
{
	m_fractalRender->OnRender();
}
