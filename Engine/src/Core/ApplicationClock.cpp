#include "Core/ApplicationClock.h"

ApplicationClock::ApplicationClock(const ClockFunction& clockFunction,
                                   int tickrate,
                                   double maxAccumulatedTime)
	: getClock(clockFunction),
	  currentTime(0.0),
	  accumulatedTime(0.0),
	  simulationTime(0.0),
	  totalTicks(0),
	  tickRate(tickrate),
	  fixedDeltaTime(1.0 / tickrate),
	  maxAccumulatedTime(maxAccumulatedTime)
{
}

void ApplicationClock::start()
{
	currentTime = getClock();
	accumulatedTime = 0.0;
	simulationTime = 0.0;
	totalTicks = 0;
}

void ApplicationClock::tick()
{
	double newTime = getClock();
	double frameTime = newTime - currentTime;

	// This is to prevent spiral of death.
	if (frameTime > maxAccumulatedTime)
	{
		frameTime = maxAccumulatedTime;
	}

	currentTime = newTime;
	accumulatedTime += frameTime;
}

bool ApplicationClock::shouldFixedUpdate() const
{
	return accumulatedTime >= fixedDeltaTime;
}

void ApplicationClock::consumeFixedUpdate()
{
	accumulatedTime -= fixedDeltaTime;
	simulationTime += fixedDeltaTime;
	totalTicks++;
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

int ApplicationClock::getTotalTicks() const
{
	return totalTicks;
}