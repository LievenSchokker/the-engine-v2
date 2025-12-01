#pragma once


#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Rendering/RenderQueue.h"

class IEngineLoop;
class ApplicationClock;

#include <atomic>
#include <memory>
#include <thread>

class SpelMotor
{
public:
    explicit SpelMotor(const ApplicationSpecifications& applicationSpecifications);
    ~SpelMotor();

    void start();
    void run();
private:
    void shutdown() const;
    bool running;

    std::unique_ptr<IEngineLoop> coreSystemLoop;
    const ApplicationSpecifications specifications;
    std::unique_ptr<ApplicationClock> coreClock;
};