#include "Core/EngineLoops/ClientLoop.h"

#include "Game.h"
#include "Core/ApplicationClock.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>

#include "Networking/NetworkSpawnManager.h"

ClientLoop::ClientLoop(std::unique_ptr<Game> spel)
    : game(std::move(spel)),
      specifications(game->getApplicationSpecifications()),
      gameWorld(std::make_unique<GameWorld>()),
      sceneManager(std::make_unique<SceneManager>()),
      client(std::make_unique<Client>(std::make_unique<TransportGNS>())),
      identityRegistry(std::make_unique<NetworkIdentityRegistry>()),
      spawnManager(nullptr)
{
    if (specifications.renderBackend == RenderBackend::SDL)
    {
        sdlContext = std::make_unique<SdlContext>();
        clockFunction = []()
        {
            return SDL_GetTicks() / 1000.0;
        };
        std::unique_ptr<IRenderer> sdlRenderer = std::make_unique<SDLRenderer>(*sdlContext);
        sdlRenderer->open(specifications.windowOptions);
        renderer = std::make_unique<RenderSystem>(std::move(sdlRenderer));
    }

    std::unique_ptr<Scene> scenePtr = game->getFirstScene();
    std::string scene = scenePtr->getName();

    sceneManager->addScene(std::move(scenePtr));
    sceneManager->setActiveScene(scene);

    spawnManager = std::make_unique<NetworkSpawnManager>(
        nullptr,
        sceneManager->getActiveScene(),
        identityRegistry.get()
    );

}

ClientLoop::~ClientLoop() = default;

void ClientLoop::start()
{
    initializeNetworking();
    inputManager = InputManager::getInstance();
}

void ClientLoop::initializeNetworking()
{
    ServerConnectionInformation serverInfo;
    serverInfo.ip = specifications.networkingOptions.serverIP;
    serverInfo.port = specifications.networkingOptions.port;

    if (!client->connectToServer(serverInfo))
    {
        std::cerr << "Failed to connect to server!" << std::endl;
        return;
    }

    auto dispatcher = spelmotor_networking::MessageDispatcherFactory::createClientDispatcher(
        *gameWorld,
        *spawnManager,
        *identityRegistry);
    client->injectMessageDispatcher(std::move(dispatcher));
}

void ClientLoop::update(double deltaTime)
{
    inputManager->update();
    renderer->update(deltaTime, *sceneManager->getActiveScene());
}

void ClientLoop::fixedUpdate(double deltaTime)
{
    client->poll();
    sceneManager->update(deltaTime, gameWorld.get());

    if (inputManager->quitRequested())
    {
        shutdown();
    }
}

void ClientLoop::shutdown()
{
    client->disconnect();
    InputManager::shutdown();
    renderer.reset();
}

GameWorld* ClientLoop::getGameWorld()
{
    return gameWorld.get();
}

SceneManager* ClientLoop::getSceneManager()
{
    return sceneManager.get();
}

ClientLoop::ClockFunction ClientLoop::getClock()
{
    return clockFunction;
}