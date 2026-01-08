#pragma once
#include "Rendering/RenderBackend.h"
#include "Rendering/Window/WindowOptions.h"
#include "Rendering/viewport/viewportConfig.h"

struct RenderSettings
{
	ViewportConfig viewport{};
	WindowOptions windowOptions{};
	RenderBackend renderBackend{};
};