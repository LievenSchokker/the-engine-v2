#pragma once

class SceneManager;
class IPhysicsWorld;
class IRenderer;
class InputManager;
class Client;
class Server;
class NetworkSpawnManager;

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
};