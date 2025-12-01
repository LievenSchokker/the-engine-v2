#include "Core/EngineLoopFactory.h"

std::unique_ptr<EngineLoop> EngineLoopFactory::createEngineLoop(EngineMode mode)
{
    switch (mode)
    {
    case EngineMode::CLIENT:
            return std::make_unique<ClientLoop>();
    case EngineMode::SERVER:
            return std::make_unique<ServerLoop>();
    //default asume client since its most likely what game dev wants.
    default:
        return std::unique_ptr<ClientLoop>();
    }
}

