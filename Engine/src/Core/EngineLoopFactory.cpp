#include "Core/EngineLoopFactory.h"
#include "Core/EngineLoops/ClientLoop.h"
#include "Core/EngineLoops/ServerLoop.h"

std::unique_ptr<IEngineLoop> EngineLoopFactory::createEngineLoop(std::unique_ptr<Game> game)
{
    switch (game->getApplicationSpecifications().networkingOptions.mode)
    {
    	case EngineMode::CLIENT:
            return std::make_unique<ClientLoop>(std::move(game));
    	case EngineMode::SERVER:
            return std::make_unique<ServerLoop>(std::move(game));
    //default asume client since its most likely what game dev wants.
    default:
        return std::make_unique<ClientLoop>(std::move(game));
    }
}

