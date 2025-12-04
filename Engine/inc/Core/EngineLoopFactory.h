#pragma once


#include "IEngineLoop.h"
#include "EngineMode.h"

struct ApplicationSpecifications;

#include <memory>

class EngineLoopFactory
{
public:
    static std::unique_ptr<IEngineLoop> createEngineLoop(ApplicationSpecifications specifications);
};
