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

SpelMotor::SpelMotor(ApplicationSpecifications applicationSpecifications)
    : specifications(applicationSpecifications)
    , tickRate(applicationSpecifications.networkingOptions.tickRate)
    , physicsWorld(std::make_unique<Box2DPhysicsWorld>(tickRate))
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
    }
}

SpelMotor::~SpelMotor()
{
    shutdown();
}

void SpelMotor::run()
{
    timer->start();
    physicsWorld->start();

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

    running = true;
    while (running)
    {
        timer->tick();

        InputManager::getInstance()->update();

        while (timer->shouldFixedUpdate())
        {
            physicsWorld->update();
            timer->consumeFixedUpdate();
        }

        renderer->presentFrame();

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