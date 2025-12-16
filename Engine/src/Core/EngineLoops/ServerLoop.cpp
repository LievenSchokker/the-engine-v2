#include "Core/EngineLoops/ServerLoop.h"

#include "Core/ApplicationClock.h"
#include "Core/EngineLoops/ClientLoop.h"
#include "Game.h"
#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/TransportGNS.h"
#include "Scene/SceneManager.h"

#include <chrono>

ServerLoop::ServerLoop(std::unique_ptr<Game> game)
	: specifications(game->getApplicationSpecifications()),
	  gameWorld(std::make_unique<GameWorld>()),
	  server(std::make_unique<Server>(
		  Server::convertApplicationSettings(specifications),
		  std::make_unique<TransportGNS>()))
{
	sceneManager = std::move(game->getSceneManager());
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
	server->start();
}

void ServerLoop::update(double deltaTime)
{
}

void ServerLoop::fixedUpdate(double deltaTime)
{
	server->update();
	sceneManager->update(deltaTime, gameWorld.get());
	currentTick++;
}

void ServerLoop::shutdown()
{
	server->stop();
}