#include "Core/EngineLoops/ServerLoop.h"
#include "Networking/Client.h"
#include "Game.h"
#include "Core/ApplicationClock.h"
#include "Core/EngineLoops/ClientLoop.h"
#include "Scene/SceneManager.h"

#include <chrono>

#include "Networking/TransportGNS.h"
#include "Networking/Server/Server.h"

ServerLoop::ServerLoop(std::unique_ptr<Game> game)
	: specifications(game->getApplicationSpecifications())
	  , gameWorld(std::make_unique<GameWorld>())
{
	clockFunction = []()
	{
		using namespace std::chrono;
		return duration<double>(steady_clock::now().time_since_epoch()).count();
	};
}

ServerLoop::~ServerLoop() = default;

ServerLoop::ClockFunction ServerLoop::getClock()
{
	return clockFunction;
}

GameWorld* ServerLoop::getGameWorld()
{
	return gameWorld.get();
}

SceneManager* ServerLoop::getSceneManager()
{
	return sceneManager.get();
}

void ServerLoop::setApplicationClock(ApplicationClock* clock)
{
	if ( gameWorld )
	{
		gameWorld->clock = clock;
	}
}

void ServerLoop::start()
{
}

void ServerLoop::update(double deltaTime)
{
}

void ServerLoop::fixedUpdate(double deltaTime)
{
	sceneManager->update(deltaTime, *gameWorld.get());
	currentTick++;
}

void ServerLoop::shutdown()
{
}