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
    explicit SpelMotor(ApplicationSpecifications applicationSpecifications);
    ~SpelMotor();

    void start();
    void run();
private:
    void shutdown() const;
    bool running;

    const ApplicationSpecifications specifications;
    std::unique_ptr<EngineLoop> coreSystemLoop;
    std::unique_ptr<ApplicationClock> coreClock;
    GameWorld gameWorld;
};