#pragma once


#include "IEngineLoop.h"
#include "EngineMode.h"

#include <memory>

class EngineLoopFactory
{
public:
    static std::unique_ptr<IEngineLoop> createEngineLoop(ApplicationSpecifications specifications);
};
