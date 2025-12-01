#pragma once
#include "ApplicationClock.h"

class SceneManager;
class IPhysicsWorld;
class IRenderer;
class InputManager;
class Client;
class Server;
class NetworkSpawnManager;
class IMessage;

class GameWorld
{
public:
    GameWorld(ApplicationClock clock);

    SceneManager* sceneManager = nullptr;
    IPhysicsWorld* physics = nullptr;
    IRenderer* renderer = nullptr;
    InputManager* input = nullptr;

    Client* client = nullptr;
    Server* server = nullptr;

    NetworkSpawnManager* spawnManager = nullptr;

    bool isServer() const { return server != nullptr; }
    bool isClient() const { return client != nullptr; }

    bool sendToServer(const IMessage& message) const;
    bool broadcastToClients(const IMessage& message) const;
    bool sendToClient(int clientId, const IMessage& message) const;

    int localClientId = -1;
};