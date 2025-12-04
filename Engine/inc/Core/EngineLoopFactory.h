#pragma once


#include "IEngineLoop.h"
#include "EngineMode.h"
#include "Game.h"

struct ApplicationSpecifications;

#include <memory>

class EngineLoopFactory
{
public:
    static std::unique_ptr<IEngineLoop> createEngineLoop(std::unique_ptr<Game> game);
};
