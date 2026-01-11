#include "Core/ApplicationClock.h"

#include <chrono>

// Use a static start time
static auto programStart = std::chrono::high_resolution_clock::now();

static double getTimeSeconds()
{
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double>(now - programStart).count();
}

ApplicationClock::ApplicationClock(const ClockFunction& clockFunc, int tickrate,
								   double maxAccTime)
	: getClock(getTimeSeconds),
	  fixedDeltaTime(1.0 / tickrate),
	  currentTime(0.0),
	  accumulatedTime(0.0),
	  simulationTime(0.0),
	  totalTicks(0),
	  tickRate(tickrate),
	  maxAccumulatedTime(maxAccTime > 0.0 ? maxAccTime : 0.25),
	  timeScale(1.0),
	  paused(false)
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
	deltaTime = frameTime;

	double oldAccum = accumulatedTime;

	if ( frameTime > maxAccumulatedTime )
	{
		frameTime = maxAccumulatedTime;
	}

	currentTime = newTime;

	// Don't accumulate time when paused to prevent teleport on resume
	// When paused, we want to freeze the simulation exactly where it is
	if ( !paused )
	{
		accumulatedTime += frameTime;
	}
}

bool ApplicationClock::shouldFixedUpdate() const
{
	return !paused && accumulatedTime >= fixedDeltaTime;
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
	return deltaTime * timeScale;
}
double ApplicationClock::getFixedDeltaTime()
{
	return fixedDeltaTime * timeScale;
}


double ApplicationClock::getAccumulatedTime() const
{
	return accumulatedTime;
}

int ApplicationClock::getTotalTicks() const
{
	return totalTicks;
}

void ApplicationClock::setTimeScale(double scale)
{
	timeScale = scale;
}

double ApplicationClock::getTimeScale() const
{
	return timeScale;
}

void ApplicationClock::pause()
{
	paused = true;
}

void ApplicationClock::resume()
{
	paused = false;
}

void ApplicationClock::togglePause()
{
	paused = !paused;
}

bool ApplicationClock::isPaused() const
{
	return paused;
}


