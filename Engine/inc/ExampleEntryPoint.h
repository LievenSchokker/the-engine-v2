#pragma once
#include "Core/SpelMotor.h"

//Example of how the engine can be ran!
namespace SpelMotorEntry
{
    inline int main(int argc, char** argv)
    {
        ApplicationSpecifications specifications{};

        specifications.windowOptions = {"SpelMotor", 700, 700};
        specifications.renderBackend = RenderBackend::SDL;

        SpelMotor engine(specifications);
        engine.run();
        return 0;
    }
}
