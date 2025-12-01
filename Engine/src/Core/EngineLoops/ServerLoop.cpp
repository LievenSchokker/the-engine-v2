#include "Core/EngineLoops/ServerLoop.h"
#include "Core/ApplicationClock.h"
#include "Scene/SceneManager.h"

#include <chrono>

#include "Networking/TransportGNS.h"
#include "Networking/Server/Server.h"

ServerLoop::ServerLoop(
	const ApplicationSpecifications& applicationspecifications)
	: applicationSpecifications(applicationspecifications)
	  , gameWorld(std::make_unique<GameWorld>())
	  , sceneManager(std::make_unique<SceneManager>())
	  , server(std::make_unique<Server>(
		  Server::convertApplicationSettings(applicationspecifications),
		  std::make_unique<TransportGNS>()))
{
	clockFunction = []()
	{
		using namespace std::chrono;
		return duration<double>(steady_clock::now().time_since_epoch()).count();
	};
}

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

void ServerLoop::update()
{
	server->update();
}

void ServerLoop::fixedUpdate(double deltaTime)
{
	currentTick++;

	if (sceneManager)
	{
		sceneManager->update(deltaTime);
	}
}

void ServerLoop::shutdown()
{
	server->stop();
}