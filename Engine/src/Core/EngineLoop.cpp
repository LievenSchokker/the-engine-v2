#include "Core/EngineLoop.h"

#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Core/IEngineSystems.h"
#include "Game.h"
#include "Input/InputManager.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <ostream>

EngineLoop::EngineLoop(std::unique_ptr<Game> game)
	: game(std::move(game))
	, gameWorld(std::make_unique<GameWorld>())
	, clockFunction([]() { return 0.0; })
{
}

EngineLoop::~EngineLoop() = default;

const ApplicationSpecifications EngineLoop::getSpecifications() const
{
	return game->getApplicationSpecifications();
}

void EngineLoop::addSystem(std::unique_ptr<IEngineSystem> system)
{
	systems.push_back(std::move(system));
}

void EngineLoop::start()
{
	std::cout << "Starting EngineLoop" << std::endl;
	for (const auto& system : systems)
	{
		system->start(*gameWorld);
	}

	getGameWorld()->input = InputManager::getInstance();
	// Cache scene manager pointer
	sceneManagerPtr = gameWorld->sceneManager;
}

void EngineLoop::update(double deltaTime)
{
	for (auto& system : systems)
	{
		system->update(deltaTime, *gameWorld);
	}
	
	getGameWorld()->input->update();
}

void EngineLoop::fixedUpdate(double deltaTime)
{
	for (auto& system : systems)
	{
		system->fixedUpdate(deltaTime, *gameWorld);
	}
}

void EngineLoop::shutdown()
{
	// Shutdown in reverse order
	for (auto it = systems.rbegin(); it != systems.rend(); ++it)
	{
		(*it)->shutdown(*gameWorld);
	}
	systems.clear();
}

bool EngineLoop::isShutdownRequested() const
{
	return shutdownRequested;
}

EngineLoop::ClockFunction EngineLoop::getClock()
{
	return clockFunction;
}

GameWorld* EngineLoop::getGameWorld()
{
	return gameWorld.get();
}

SceneManager* EngineLoop::getSceneManager()
{
	return sceneManagerPtr;
}

void EngineLoop::setApplicationClock(ApplicationClock* clock)
{
	if (gameWorld)
	{
		gameWorld->clock = clock;
	}
}