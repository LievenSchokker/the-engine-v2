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
      , serverRegistry(std::make_unique<NetworkIdentityRegistry>())
      , sceneManager(std::make_unique<SceneManager>())
      , server(std::make_unique<Server>(
          Server::convertApplicationSettings(specifications),
          std::make_unique<TransportGNS>())),
         spawnManager(nullptr)
      , gameWorld(std::make_unique<GameWorld>())
{
    std::unique_ptr<Scene> scenePtr = game->getFirstScene();
    std::string sceneName = scenePtr->getName();

    spawnManager = std::make_unique<NetworkSpawnManager>(
        server.get(),
        scenePtr.get(),
        serverRegistry.get()
    );

    sceneManager->addScene(std::move(scenePtr));
    sceneManager->setActiveScene(sceneName);
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