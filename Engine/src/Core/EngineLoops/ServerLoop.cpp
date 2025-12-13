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
	  , server(std::make_unique<Server>(
          Server::convertApplicationSettings(specifications),
          std::make_unique<TransportGNS>()))
	  , gameWorld(std::make_unique<GameWorld>())
	  , sceneManager(std::make_unique<SceneManager>(*gameWorld))
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