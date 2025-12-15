#pragma once


class Server;
class Client;
class SceneManager;
class RenderSystem;
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
    RenderSystem* render = nullptr;
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