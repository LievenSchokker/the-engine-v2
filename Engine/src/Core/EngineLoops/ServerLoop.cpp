#include "Core/EngineLoops/ServerLoop.h"

#include "Core/ApplicationClock.h"
#include "Core/EngineLoops/ClientLoop.h"
#include "Game.h"
#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/TransportGNS.h"
#include "Game.h"
#include "Core/ApplicationClock.h"
#include "Input/InputManager.h"
#include "Scene/SceneManager.h"

#include <chrono>

#include "Networking/TransportGNS.h"
#include "Networking/Server/Server.h"
#include "Networking/Server/StateSyncSystem.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/Messages/ConcreteMessages/WelcomeMessage.h"

ServerLoop::ServerLoop(std::unique_ptr<Game> game)
    : specifications(game->getApplicationSpecifications())
      , sceneManager(game->getSceneManager())
      , server(std::make_unique<Server>(
          Server::convertApplicationSettings(specifications),
          std::make_unique<TransportGNS>()))
      , spawnManager(nullptr)
      , stateSync(nullptr)
      , gameWorld(std::make_unique<GameWorld>())
{
    clockFunction = []()
    {
        using namespace std::chrono;
        return duration<double>(steady_clock::now().time_since_epoch()).count();
    };

    gameWorld->server = server.get();
    gameWorld->sceneManager = sceneManager.get();
    gameWorld->input = InputManager::getInstance();
    sceneManager->setWorld(gameWorld.get());

    spawnManager = std::make_unique<NetworkSpawnManager>(gameWorld.get());
    gameWorld->spawnManager = spawnManager.get();

    sceneManager->configureNetworking(ConnectionMode::Host, spawnManager.get());

    stateSync = std::make_unique<StateSyncSystem>(
        server.get(),
        &spawnManager->getNetworkIdentityRegistry()
    );
}

ServerLoop::ServerLoop(std::unique_ptr<Game> game, std::unique_ptr<ITransport> transport)
    : specifications(game->getApplicationSpecifications())
    , sceneManager(std::make_unique<SceneManager>())
    , server(std::make_unique<Server>(
          Server::convertApplicationSettings(specifications),
          std::move(transport)))
    , spawnManager(nullptr)
    , stateSync(nullptr)
    , gameWorld(std::make_unique<GameWorld>())
{
    clockFunction = []()
    {
        using namespace std::chrono;
        return duration<double>(steady_clock::now().time_since_epoch()).count();
    };


    gameWorld->server = server.get();
    gameWorld->sceneManager = sceneManager.get();
    gameWorld->input = InputManager::getInstance();
    sceneManager->setWorld(gameWorld.get());

    spawnManager = std::make_unique<NetworkSpawnManager>(gameWorld.get());
    gameWorld->spawnManager = spawnManager.get();

    sceneManager->configureNetworking(ConnectionMode::Host, spawnManager.get());

    stateSync = std::make_unique<StateSyncSystem>(
        server.get(),
        &spawnManager->getNetworkIdentityRegistry()
    );

    if (sceneManager->getActiveScene() == nullptr)
    {
        std::string sceneName = sceneManager->getFirstSceneName();
        if (!sceneName.empty())
        {
            sceneManager->setActiveScene(sceneName);
        }
    }
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
	initializeNetworking();
	server->start();
}

void ServerLoop::initializeNetworking()
{
    // Removed NetworkContext parameter
	auto dispatcher =
		spelmotorNetworking::MessageDispatcherFactory::createServerDispatcher(
			*gameWorld,
			*spawnManager,
				spawnManager->getNetworkIdentityRegistry());
	server->injectMessageDispatcher(std::move(dispatcher));
}

void ServerLoop::update(double deltaTime)
{
}

void ServerLoop::fixedUpdate(double deltaTime)
{
	server->update();
	sceneManager->update(deltaTime, gameWorld.get());
	stateSync->tick(currentTick);
	currentTick++;
}

void ServerLoop::shutdown()
{
	server->stop();
}