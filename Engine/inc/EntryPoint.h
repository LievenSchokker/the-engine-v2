#pragma once

#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"

#include <cstring>
#include <iostream>

#undef main

namespace SpelMotorEntry
{
    inline int main(int argc, char** argv)
    {
        ApplicationSpecifications specifications{};

        specifications.windowOptions = {"SpelMotor", 700, 700};
        specifications.renderBackend = RenderBackend::SDL;
        specifications.networkingOptions.tickRate = 60;
        specifications.networkingOptions.port = 7777;
        specifications.networkingOptions.serverIP = "127.0.0.1";
        specifications.networkingOptions.mode = EngineMode::CLIENT;
        
        if (argc > 1)
        {
            if (std::strcmp(argv[1], "server") == 0)
            {
                specifications.networkingOptions.mode = EngineMode::SERVER;
                std::cout << "Starting SERVER on port 7777\n";
            }
            else if (std::strcmp(argv[1], "client") == 0)
            {
                specifications.networkingOptions.mode = EngineMode::CLIENT;
                std::cout << "Starting CLIENT connecting to 127.0.0.1:7777\n";
            }
        }
        else
        {
            std::cout << "Starting CLIENT connecting to 127.0.0.1:7777 (default)\n";
        }

        SpelMotor engine(specifications);
        engine.run();
        return 0;
    }
}