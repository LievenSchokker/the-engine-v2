#pragma once

#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Rendering/RenderQueue.h"

#include <atomic>
#include <memory>
#include <thread>

// Forward declarations
class ApplicationClock;
class IPhysicsWorld;
class IRenderer;
class SceneManager;
class Server;
class Client;
class NetworkSpawnManager;

class SpelMotor
{
public:
    SpelMotor(ApplicationSpecifications applicationSpecifications);
    ~SpelMotor();

    void run();

    SceneManager* getSceneManager();
    GameWorld* getGameWorld() { return &gameWorld; }

private:
    void runClient();
    void runServer();
    void startNetworkThread();
    void stopNetworkThread();
    void shutdown();

    void initializeNetworking();

    int tickRate;
    std::atomic<bool> running{false};

    const ApplicationSpecifications specifications;

    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<ApplicationClock> timer;
    std::unique_ptr<IPhysicsWorld> physicsWorld;
    std::unique_ptr<SceneManager> sceneManager;

    std::unique_ptr<Server> server;
    std::unique_ptr<Client> client;
    std::unique_ptr<NetworkSpawnManager> spawnManager;

    std::thread networkThread;
    std::atomic<bool> networkRunning{false};

    GameWorld gameWorld;
};