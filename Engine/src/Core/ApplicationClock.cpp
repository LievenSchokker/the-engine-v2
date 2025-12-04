#include "Core/ApplicationClock.h"

#include <iostream>

#include "Core/ApplicationClock.h"
#include <chrono>
#include <iostream>

// Use a static start time
static auto programStart = std::chrono::high_resolution_clock::now();

static double getTimeSeconds()
{
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double>(now - programStart).count();
}

ApplicationClock::ApplicationClock(const ClockFunction& clockFunc,
							int tickrate, double maxAccTime)
	: getClock(getTimeSeconds),
	  fixedDeltaTime(1.0 / tickrate),
	  currentTime(0.0),
	  accumulatedTime(0.0),
	  simulationTime(0.0),
	  totalTicks(0),
	  tickRate(tickrate),
	  maxAccumulatedTime(maxAccTime > 0.0 ? maxAccTime : 0.25)
{
}

void ApplicationClock::start()
{
	currentTime = getClock();
}

void ApplicationClock::tick()
{
	double newTime = getClock();
	double frameTime = newTime - currentTime;

	double oldAccum = accumulatedTime;

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