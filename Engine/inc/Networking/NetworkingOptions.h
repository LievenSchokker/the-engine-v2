#pragma once


#include <cstdint>
#include <string>
#include "Core/EngineMode.h"


struct NetworkingOptions
{
    EngineMode mode;
    int tickRate;
    uint16_t port;
    std::string serverIP;
};
