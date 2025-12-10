<<<<<<< HEAD
#include <string>

#include "Core/EngineMode.h"

=======
#pragma once


#include <string>
#include "Core/EngineMode.h"


>>>>>>> origin/development
struct NetworkingOptions
{
    EngineMode mode;
    int tickRate;
    int port;
    std::string serverIP;
};
