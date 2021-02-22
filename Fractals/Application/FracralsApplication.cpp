#include "FracralsApplication.h"

#include "Logger/Logger.h"

#if defined(WIN32)
#include "Application/Win32App.h"
#endif

#include "Graphics/FractalsRender.h"

FracralsApplication::FracralsApplication()
	: m_app(new Win32App())
	, m_fractalRender(new FractalsRender())
{
}

FracralsApplication::~FracralsApplication()
{
}

void FracralsApplication::Init()
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

	m_app->SetUpdateCallback(std::bind(&FracralsApplication::OnUpdate, this, std::placeholders::_1));
	m_app->SetRenderCallback(std::bind(&FracralsApplication::OnRender, this));
	m_app->SetWindowSizeChanged([this](const math::vec2f& newSize)
	{
		m_fractalRender->OnWindowSizeChanged(newSize);
	});

	m_app->Init();
	m_fractalRender->Init();
	Logger::Log(LogLevel::INFO, "Initialization finished");
}

int FracralsApplication::Run()
{
	if (m_app == nullptr)
	{
		return -1;
	}
	return m_app->Run();
}

void FracralsApplication::OnUpdate(float dt)
{
	m_fractalRender->OnUpdate(dt);
}

void FracralsApplication::OnRender()
{
	m_fractalRender->OnRender();
}
