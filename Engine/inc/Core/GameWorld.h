#pragma once

#include "Events/EventDispatcher/EventDispatcher.h"
#include "Options/ApplicationSpecifications.h"

#include <memory>

#include "Assets/AssetManager.h"

class ApplicationClock;
class AudioManager;
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

	// Non owning pointer to different systems
	SceneManager* sceneManager = nullptr;
	IPhysicsWorld* physics = nullptr;
	RenderSystem* render = nullptr;
	InputManager* input = nullptr;
	ApplicationClock* clock = nullptr;
	AudioManager* audio = nullptr;
	Client* client = nullptr;
	Server* server = nullptr;
	NetworkSpawnManager* spawnManager = nullptr;
	ApplicationSpecifications specs = {};

	bool isClient() const
	{
		return client != nullptr;
	}

	bool isServer() const
	{
		return server != nullptr;
	}

	bool sendToServer(const IMessage& message);
	bool broadcastToClients(const IMessage& message);
	bool sendToClient(int clientId, const IMessage& message);
	void setDispatcher(std::unique_ptr<EventDispatcher> dispatcher);
	EventDispatcher* getDispatcher();
    void setAssetManager(std::unique_ptr<AssetManager> asset_manager);
    AssetManager* getAssetManager();
    int localClientId = -1;
private:
    std::unique_ptr<AssetManager> assetManager = nullptr;
	std::unique_ptr<EventDispatcher> dispatcher = nullptr;
};