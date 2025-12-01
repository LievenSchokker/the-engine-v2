#pragma once


#include "ApplicationSpecifications.h"
#include "GameWorld.h"

class IEngineLoop
{
	using ClockFunction = std::function<double()>;

public:
	virtual ~IEngineLoop() = default;

	virtual GameWorld* getGameWorld() = 0;
	virtual SceneManager* getSceneManager() = 0;
	virtual ApplicationClock::ClockFunction getClock() = 0;
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void fixedUpdate(double fixedDeltaTime) = 0;
	virtual void shutdown() = 0;
};