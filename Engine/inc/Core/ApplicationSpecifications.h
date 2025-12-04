#pragma once


#include "Networking/NetworkingOptions.h"
#include "Rendering/Window/WindowOptions.h"
#include "Rendering/RenderBackend.h"

struct ApplicationSpecifications
{
    NetworkingOptions networkingOptions;
    WindowOptions windowOptions;
    RenderBackend renderBackend;
    double maxFrameTime;
};
