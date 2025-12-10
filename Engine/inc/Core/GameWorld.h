#pragma once

<<<<<<< HEAD
class SceneManager;
class IPhysicsWorld;
class IRenderer;
class InputManager;
class Client;
class Server;
=======

class Server;
class Client;
class IRenderer;
class SceneManager;
class IPhysicsWorld;
class InputManager;
>>>>>>> origin/development
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

<<<<<<< HEAD
    bool sendToServer(const IMessage& message);
    bool broadcastToClients(const IMessage& message);
    bool sendToClient(int clientId, const IMessage& message);
=======
    bool sendToServer(const IMessage& message) const;
    bool broadcastToClients(const IMessage& message) const;
    bool sendToClient(int clientId, const IMessage& message) const;
>>>>>>> origin/development

    int localClientId = -1;
};