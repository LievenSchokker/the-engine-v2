#pragma once


#include "SystemStatus.h"

#include <string>

class GameWorld;

class IEngineSystem
{
public:
	virtual ~IEngineSystem() = default;

	virtual SystemStatus start(GameWorld& gameWorld)
	{
		return SystemStatus::Error;
	}

	virtual void update(double deltaTime, const GameWorld& gameWorld)
	{
	}

	virtual void fixedUpdate(double deltaTime, const GameWorld& gameWorld)
	{
	}

	virtual void shutdown(GameWorld& gameWorld)
	{
	}

	virtual const std::string getName() const = 0;
};