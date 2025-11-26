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
#include "Networking/NetworkSpawnManager.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <chrono>

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

        gameWorld.server = server.get();
    }

    gameWorld.sceneManager = sceneManager.get();
    gameWorld.physics = physicsWorld.get();
    gameWorld.input = InputManager::getInstance();

    sceneManager->setWorld(&gameWorld);
}

SpelMotor::~SpelMotor()
{
    shutdown();
}

void SpelMotor::initializeNetworking()
{
    // Get active scene for spawn manager
    Scene* activeScene = sceneManager->getActiveScene();
    if (!activeScene)
    {
        std::cerr << "[SpelMotor] No active scene for networking" << std::endl;
        return;
    }

    if (server)
    {
        // Server: create spawn manager
        spawnManager = std::make_unique<NetworkSpawnManager>(server.get(), activeScene);
        gameWorld.spawnManager = spawnManager.get();

        // Create and inject message dispatcher
        auto dispatcher = spelmotor_networking::MessageDispatcherFactory::createServerDispatcher(
            gameWorld, *spawnManager);
        server->injectMessageDispatcher(std::move(dispatcher));
    }
    else if (client)
    {
        // Client: create spawn manager (no server pointer)
        spawnManager = std::make_unique<NetworkSpawnManager>(nullptr, activeScene);
        gameWorld.spawnManager = spawnManager.get();

        // Create and inject message dispatcher
        auto dispatcher = spelmotor_networking::MessageDispatcherFactory::createClientDispatcher(
            gameWorld, *spawnManager);
        client->injectMessageDispatcher(std::move(dispatcher));
    }
}

void SpelMotor::run()
{
    timer->start();
    physicsWorld->start();

    // Initialize networking after scene is set
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
        std::cerr << "Failed to connect to server" << std::endl;
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
            if (sceneManager)
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

void SpelMotor::runServer()
{
    if (server->start() != ServerStatus::Running)
    {
        std::cerr << "Failed to start server" << std::endl;
        return;
    }

    running = true;
    while (running)
    {
        timer->tick();
        server->update();

        while (timer->shouldFixedUpdate())
        {
            if (sceneManager)
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
    if (!running.exchange(false))
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
    return sceneManager.get();
}

void Server::setClientConnectedCallback(ClientConnectedCallback callback)
{
    onClientConnected = std::move(callback);
}

void Server::setClientDisconnectedCallback(ClientDisconnectedCallback callback)
{
    onClientDisconnected = std::move(callback);
}

void Server::onConnectionChanged(const Connection& connection)
{
    if (connection.connectionStatus == ConnectionStatus::Connected)
    {
        connectedClients.insert(connection.transportConnectionId);
        std::cout << "[Server] Client " << connection.transportConnectionId << " connected" << std::endl;

        if (onClientConnected)
        {
            onClientConnected(connection.transportConnectionId);
        }
    }
    else if (connection.connectionStatus == ConnectionStatus::Disconnected)
    {
        connectedClients.erase(connection.transportConnectionId);
        std::cout << "[Server] Client " << connection.transportConnectionId << " disconnected" << std::endl;

        if (onClientDisconnected)
        {
            onClientDisconnected(connection.transportConnectionId);
        }
    }
}