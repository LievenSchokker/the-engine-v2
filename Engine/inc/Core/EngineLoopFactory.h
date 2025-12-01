#pragma once


#include "EngineLoop.h"
#include "EngineMode.h"

#include <memory>

class EngineLoopFactory
{
public:
    static std::unique_ptr<EngineLoop> createEngineLoop(ApplicationSpecifications specifications, GameWorld* gameWorld);
};
