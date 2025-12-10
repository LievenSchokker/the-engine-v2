#include "Core/EngineLoops/ServerLoop.h"
#include "Game.h"
#include "Core/ApplicationClock.h"
#include "Scene/SceneManager.h"

#include <chrono>

#include "Networking/TransportGNS.h"
#include "Networking/Server/Server.h"
#include "Networking/Server/StateSyncSystem.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/Messages/ConcreteMessages/WelcomeMessage.h"

ServerLoop::ServerLoop(std::unique_ptr<Game> game)
    : specifications(game->getApplicationSpecifications())
      , serverRegistry(std::make_unique<NetworkIdentityRegistry>())
      , sceneManager(std::make_unique<SceneManager>())
      , server(std::make_unique<Server>(
          Server::convertApplicationSettings(specifications),
          std::make_unique<TransportGNS>()))
      , spawnManager(nullptr)
      , stateSync(nullptr)  // Add to initializer list
      , gameWorld(std::make_unique<GameWorld>())
{
    clockFunction = []()
    {
        using namespace std::chrono;
        return duration<double>(steady_clock::now().time_since_epoch()).count();
    };

    std::unique_ptr<Scene> scenePtr = game->getFirstScene();
    std::string sceneName = scenePtr->getName();

    sceneManager->addScene(std::move(scenePtr));
    sceneManager->setActiveScene(sceneName);

    spawnManager = std::make_unique<NetworkSpawnManager>(
        server.get(),
        sceneManager->getActiveScene(),
        serverRegistry.get()
    );

    // Create state sync system
    stateSync = std::make_unique<StateSyncSystem>(
        server.get(),
        serverRegistry.get()
    );

    gameWorld->server = server.get();
    gameWorld->sceneManager = sceneManager.get();
    gameWorld->spawnManager = spawnManager.get();

    sceneManager->setWorld(gameWorld.get());
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
    initializeNetworking();
    server->start();
}

void ServerLoop::initializeNetworking()
{
    server->setClientConnectedCallback([this](int clientId)
    {
        WelcomeMessage welcome(clientId);
        server->sendMessage(clientId, welcome);

        spawnManager->syncExistingObjects(clientId);
        Vector2 spawnPos{350.0f + (clientId * 60.0f), 350.0f};
        spawnManager->spawnPlayer(clientId, spawnPos);
    });

    server->setClientDisconnectedCallback([this](int clientId)
    {
        spawnManager->despawnClientObjects(clientId);
    });

    // Removed NetworkContext parameter
    auto dispatcher = spelmotor_networking::MessageDispatcherFactory::createServerDispatcher(
        *gameWorld,
        *spawnManager,
        *serverRegistry);
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