#pragma once


#include <string>
#include "Core/EngineMode.h"

struct NetworkingOptions
{
    EngineMode mode;
    int tickRate;
    int port;
    std::string serverIP;
};
