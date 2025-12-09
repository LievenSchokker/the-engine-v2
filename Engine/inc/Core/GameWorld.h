#pragma once
#include "Events/EventDispatcher.h"


class Server;
class Client;
class IRenderer;
class SceneManager;
class IPhysicsWorld;
class InputManager;
class NetworkSpawnManager;
class IMessage;

class GameWorld
{
public:
    GameWorld() = default;

    SceneManager* sceneManager = nullptr;
    IPhysicsWorld* physics = nullptr;
    IRenderer* renderer = nullptr;
    InputManager* input = nullptr;
	EventDispatcher* dispatcher = nullptr;
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