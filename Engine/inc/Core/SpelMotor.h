#pragma once


#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Rendering/RenderQueue.h"

class EngineLoop;
class ApplicationClock;

#include <atomic>
#include <memory>
#include <thread>

class SpelMotor
{
public:
    SpelMotor(ApplicationSpecifications applicationSpecifications);
    ~SpelMotor();

    void start();
    void run();
private:
    void shutdown();
    int tickRate;

    const ApplicationSpecifications specifications;
    std::unique_ptr<EngineLoop> coreSystemLoop;
    GameWorld gameWorld;
};