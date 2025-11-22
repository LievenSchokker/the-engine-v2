#pragma once


#include "../Rendering/Window/WindowOptions.h"
#include "Rendering/RenderBackend.h"


struct ApplicationSpecifications
{
    WindowOptions windowOptions;
    RenderBackend renderBackend;
};
