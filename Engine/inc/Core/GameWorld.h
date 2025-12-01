#pragma once

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
    GameWorld() = default;

    SceneManager* sceneManager = nullptr;
    IPhysicsWorld* physics = nullptr;
    IRenderer* renderer = nullptr;
    InputManager* input = nullptr;

    Client* client = nullptr;
    Server* server = nullptr;

    NetworkSpawnManager* spawnManager = nullptr;

    bool isServer() const { return server != nullptr; }
    bool isClient() const { return client != nullptr; }

    bool sendToServer(const IMessage& message);
    bool broadcastToClients(const IMessage& message);
    bool sendToClient(int clientId, const IMessage& message);

    int localClientId = -1;
};