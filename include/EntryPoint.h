#pragma once

#include "Application.h"

extern Application::Application* Application::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace Spelmotor
{
int Main(int argc, char** argv)
{
	while (g_ApplicationRunning) {
		InitializeCore();
		Application* app = CreateApplication(argc, argv);
		HZ_CORE_ASSERT(app, "Client Application is null!");
		app->Run();
		delete app;
		ShutdownCore();
	}
	return 0;
}
}