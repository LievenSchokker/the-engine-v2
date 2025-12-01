#include "Core/EngineLoopFactory.h"
#include "Core/EngineLoops/ClientLoop.h"
#include "Core/EngineLoops/ServerLoop.h"

std::unique_ptr<IEngineLoop> EngineLoopFactory::createEngineLoop(ApplicationSpecifications specifications)
{
    switch (specifications.networkingOptions.mode)
    {
    case EngineMode::CLIENT:
            return std::make_unique<ClientLoop>(specifications);
    case EngineMode::SERVER:
            return std::make_unique<ServerLoop>(specifications);
    //default asume client since its most likely what game dev wants.
    default:
        return std::make_unique<ClientLoop>(specifications);
    }
}

