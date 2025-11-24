

#include "Core/ApplicationClock.h"


#include <algorithm>


ApplicationClock::ApplicationClock(double fixedDeltaTime, ClockFunction clockFunc)
    : getClock(clockFunc),
      fixedDeltaTime(fixedDeltaTime),
      currentTime(0.0),
      accumulatedTime(0.0),
      simulationTime(0.0),
      tickRate(0)
{
}

void ApplicationClock::start()
{
    currentTime = getClock();
    accumulatedTime = 0.0;
    simulationTime = 0.0;
    tickRate = 0;
}

void ApplicationClock::tick()
{
    double newTime = getClock();
    double frameTime = newTime - currentTime;

    // This is to prevent spiral of death.
    if (frameTime > 0.25)
    {
        frameTime = 0.25;
    }

    currentTime = newTime;
    accumulatedTime += frameTime;
}

bool ApplicationClock::shouldFixedUpdate()
{
    return accumulatedTime >= fixedDeltaTime;
}

void ApplicationClock::consumeFixedUpdate()
{
    accumulatedTime -= fixedDeltaTime;
    simulationTime += fixedDeltaTime;
    tickRate++;
}

double ApplicationClock::getAlpha() const
{
    return accumulatedTime / fixedDeltaTime;
}

double ApplicationClock::getTime() const
{
    return simulationTime;
}

double ApplicationClock::getDeltaTime() const
{
    return fixedDeltaTime;
}

double ApplicationClock::getAccumulatedTime() const
{
    return accumulatedTime;
}

int ApplicationClock::getTickRate() const
{
    return tickRate;
}