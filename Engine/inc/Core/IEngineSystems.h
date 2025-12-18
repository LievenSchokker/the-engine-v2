#pragma once


#include "SystemStatus.h"

#include <string>

class GameWorld;

class IEngineSystems
{
public:
	virtual ~IEngineSystems() = default;

	virtual SystemStatus start(GameWorld& gameWorld)
	{
		return SystemStatus::ERROR;
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