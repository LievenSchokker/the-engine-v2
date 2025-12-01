#include "Core/EngineLoopFactory.h"

#include "Core/EngineLoops/ClientLoop.h"
#include "Core/EngineLoops/ServerLoop.h"

std::unique_ptr<EngineLoop> EngineLoopFactory::createEngineLoop(ApplicationSpecifications specifications, GameWorld* gameWorld)
{
    switch (mode)
    {
    case EngineMode::CLIENT:
            return std::make_unique<ClientLoop>(specifications, gameWorld);
    case EngineMode::SERVER:
            return std::make_unique<ServerLoop>(specifications, gameWorld);
    //default asume client since its most likely what game dev wants.
    default:
        return std::make_unique<ClientLoop>(specifications, gameWorld);
    }
}

