#pragma once

#include "SpelMotor.h"

inline int Main(int argc, char** argv)
{
    ApplicationSpecifications specifications {};

    specifications.windowOptions = {"SpelMotor", 1920, 1080};
    specifications.renderBackend = RenderBackend::SDL;

    SpelMotor engine(specifications);
    engine.run();
    return 0;
}