// Timer.cpp
#include "Core/Timer.h"
#include <algorithm>

Timer::Timer(double fixedDeltaTime, ClockFunction clockFunc)
    : getClock(clockFunc),
      fixedDeltaTime(fixedDeltaTime),
      currentTime(0.0),
      accumulatedTime(0.0),
      simulationTime(0.0),
      tickRate(0)
{
}

void Timer::start()
{
    currentTime = getClock();
    accumulatedTime = 0.0;
    simulationTime = 0.0;
    tickRate = 0;
}

void Timer::tick()
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

bool Timer::shouldFixedUpdate()
{
    return accumulatedTime >= fixedDeltaTime;
}

void Timer::consumeFixedUpdate()
{
    accumulatedTime -= fixedDeltaTime;
    simulationTime += fixedDeltaTime;
    tickRate++;
}

double Timer::getAlpha() const
{
    return accumulatedTime / fixedDeltaTime;
}

double Timer::getTime() const
{
    return simulationTime;
}

double Timer::getDeltaTime() const
{
    return fixedDeltaTime;
}

double Timer::getAccumulatedTime() const
{
    return accumulatedTime;
}

int Timer::getTickRate() const
{
    return tickRate;
}