#include "Core/SpelMotor.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Networking/Server/Server.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"

#include <iostream>
#include <chrono>

#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Scene/SceneManager.h"
#include "Networking/Messages/Concretes/WelcomeMessage.h"

SpelMotor::SpelMotor(ApplicationSpecifications applicationSpecifications)
    : specifications(applicationSpecifications)
    , tickRate(applicationSpecifications.networkingOptions.tickRate)
    , physicsWorld(std::make_unique<Box2DPhysicsWorld>(tickRate))
    , sceneManager(std::make_unique<SceneManager>())
{
    const auto& netOpts = specifications.networkingOptions;

    if (netOpts.mode == EngineMode::CLIENT)
    {
        if (specifications.renderBackend == RenderBackend::SDL)
        {
            SdlContext context = SdlContext();
            timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []()
            {
                return SDL_GetTicks() / 1000.0;
            });
            renderer = std::make_unique<SDLRenderer>(context);
        }

        client = std::make_unique<Client>(std::make_unique<TransportGNS>());

        // Set AFTER creating objects
        gameWorld.renderer = renderer.get();
        gameWorld.client = client.get();
    }
    else
    {
        timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []()
        {
            using namespace std::chrono;
            return duration<double>(steady_clock::now().time_since_epoch()).count();
        });

        ServerConnectionInformation serverInfo;
        serverInfo.port = netOpts.port;
        server = std::make_unique<Server>(serverInfo, std::make_unique<TransportGNS>());

        // Set AFTER creating server
        gameWorld.server = server.get();
    }

    // Common initialization - after all objects exist
    gameWorld.sceneManager = sceneManager.get();
    gameWorld.physics = physicsWorld.get();
    gameWorld.input = InputManager::getInstance();

    sceneManager->setWorld(&gameWorld);
}

SpelMotor::~SpelMotor()
{
    shutdown();
}

void SpelMotor::run()
{
    timer->start();
    physicsWorld->start();

    initializeNetworking();
    if (specifications.networkingOptions.mode == EngineMode::CLIENT)
    {
        runClient();
    }
    else
    {
        runServer();
    }
}

void SpelMotor::runClient()
{
    renderer->open(specifications.windowOptions);

    ServerConnectionInformation serverInfo;
    serverInfo.ip = specifications.networkingOptions.serverIP;
    serverInfo.port = specifications.networkingOptions.port;

    if (!client->connectToServer(serverInfo))
    {
        return;
    }

    startNetworkThread();
    RenderQueue renderQueue;
    running = true;
    while (running)
    {
        timer->tick();
        InputManager::getInstance()->update();

        while (timer->shouldFixedUpdate())
        {
            if (sceneManager != nullptr)
            {
                sceneManager->update(timer->getDeltaTime());
            }

            physicsWorld->update();
            timer->consumeFixedUpdate();
        }
        sceneManager->buildRenderQueue(renderQueue);
        renderer->render(renderQueue);
        if (InputManager::getInstance()->quitRequested())
        {
            shutdown();
        }
    }
}

void SpelMotor::initializeNetworking()
{
    Scene* activeScene = sceneManager->getActiveScene();
    if (!activeScene)
    {
        return;
    }

    // Create identity registry
    identityRegistry = std::make_unique<NetworkIdentityRegistry>();

    if (server)
    {
        spawnManager = std::make_unique<NetworkSpawnManager>(server.get(), activeScene, identityRegistry.get());
        gameWorld.spawnManager = spawnManager.get();

        server->setClientConnectedCallback([this](int clientId) {

            WelcomeMessage welcome(clientId);
            server->sendMessage(clientId, welcome);

            spawnManager->syncExistingObjects(clientId);
            Vector2 spawnPos{350.0f + (clientId * 60.0f), 350.0f};
            spawnManager->spawnPlayer(clientId, spawnPos);
        });

        server->setClientDisconnectedCallback([this](int clientId) {
            spawnManager->despawnClientObjects(clientId);
        });

        auto dispatcher = spelmotor_networking::MessageDispatcherFactory::createServerDispatcher(
            gameWorld,
            *spawnManager,
            server->getNetworkContext(),  // Get context from server
            *identityRegistry);
        server->injectMessageDispatcher(std::move(dispatcher));
    }
    else if (client)
    {
        spawnManager = std::make_unique<NetworkSpawnManager>(server.get(), activeScene, identityRegistry.get());
        gameWorld.spawnManager = spawnManager.get();

        auto dispatcher = spelmotor_networking::MessageDispatcherFactory::createClientDispatcher(
            gameWorld,
            *spawnManager,
            client->getNetworkContext(),  // Get context from client
            *identityRegistry);
        client->injectMessageDispatcher(std::move(dispatcher));
    }
}

void SpelMotor::runServer()
{
    if (server->start() != ServerStatus::Running)
    {
        return;
    }

    running = true;
    while (running)
    {
        timer->tick();
        server->update();

        while (timer->shouldFixedUpdate())
        {
            if (sceneManager != nullptr)
            {
                sceneManager->update(timer->getDeltaTime());
            }

            physicsWorld->update();
            timer->consumeFixedUpdate();
        }
    }
}

void SpelMotor::startNetworkThread()
{
    networkRunning = true;
    networkThread = std::thread([this]()
    {
        while (networkRunning)
        {
            client->poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}

void SpelMotor::stopNetworkThread()
{
    networkRunning = false;
    if (networkThread.joinable())
    {
        networkThread.join();
    }
}

void SpelMotor::shutdown()
{
    if (!networkRunning.exchange(false))
    {
        return;
    }

    stopNetworkThread();

    if (client)
    {
        client->disconnect();
    }

    if (server)
    {
        server->stop();
    }

    if (renderer)
    {
        InputManager::shutdown();
        renderer->close();
    }

    physicsWorld->shutdown();
}


SceneManager* SpelMotor::getSceneManager()
{
    if (sceneManager) return sceneManager.get();
    return nullptr;
}
