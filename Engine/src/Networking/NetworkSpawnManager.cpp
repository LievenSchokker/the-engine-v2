#include "Networking/NetworkSpawnManager.h"
#include "../../inc/Component/NetworkIdentity.h"
#include "Networking/NetworkPrefabRegistry.h"
#include "Networking/Server/Server.h"
#include "Networking/Messages/ConcreteMessages/SpawnMessage.h"
#include "Networking/Messages/ConcreteMessages/ObjectDestroyMessage.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Component/Transform.h"
#include "Networking/PrefabLibrary.h"
#include "Scene/SceneManager.h"

#include <iostream>

NetworkSpawnManager::NetworkSpawnManager(GameWorld* gameWorlds)
	: server(gameWorlds->server)
	, identityRegistry(std::make_unique<NetworkIdentityRegistry>())
	, prefabLibrary(std::make_unique<PrefabLibrary>())
	, gameWorld(gameWorlds)
{
}


uint32_t NetworkSpawnManager::generateNetId()
{
	return nextNetworkId++;
}

GameObject* NetworkSpawnManager::spawnObject(const uint32_t assetId, const Vector2 position, const int ownerId)
{
    std::cerr << "[spawnObject] assetId=" << assetId << " position=(" << position.x << ", " << position.y << ")\n";

    if (!prefabLibrary)
    {
        std::cerr << "[spawnObject] No prefabLibrary!\n";
        return nullptr;
    }

    std::cerr << "[spawnObject] PrefabLibrary size=" << prefabLibrary->size() << "\n";

    auto gameObject = prefabLibrary->instantiate(assetId);
    if (!gameObject)
    {
        std::cerr << "[spawnObject] Failed to instantiate assetId=" << assetId << "\n";
        return nullptr;
    }

    std::cerr << "[spawnObject] Instantiated successfully\n";

    std::cerr << "[spawnObject] Setting position...\n";
    gameObject->getTransform()->setPosition(position);

    std::cerr << "[spawnObject] Getting NetworkIdentity...\n";
    auto* identity = gameObject->getComponent<NetworkIdentity>();
    if (!identity)
    {
        std::cerr << "[spawnObject] Adding NetworkIdentity...\n";
        identity = gameObject->addComponent<NetworkIdentity>();
    }

    std::cerr << "[spawnObject] Generating netId...\n";
    const uint32_t networkId = generateNetId();

    std::cerr << "[spawnObject] Setting identity fields (netId=" << networkId << ")...\n";
    identity->networkId = networkId;
    identity->ownerId = ownerId;

    std::cerr << "[spawnObject] Setting gameWorld...\n";
    identity->gameWorld = gameWorld;

    std::cerr << "[spawnObject] Calling onNetworkSpawn (first)...\n";
    identity->onNetworkSpawn();

    std::cerr << "[spawnObject] Registering identity...\n";
    if (identityRegistry)
    {
        identityRegistry->registerIdentity(identity);
    }

    std::cerr << "[spawnObject] Storing pointers...\n";
    GameObject* gameObjectPointer = gameObject.get();
    spawnedObjects[networkId] = gameObjectPointer;
    objectAssets[networkId] = assetId;

    if (ownerId >= 0)
    {
        clientOwnedObjects[ownerId].push_back(networkId);
    }

    std::cerr << "[spawnObject] Checking scene...\n";
    if (!getScene())
    {
        return nullptr;
    }

    std::cerr << "[spawnObject] Adding to scene...\n";
    getScene()->addGameObject(std::move(gameObject));

    std::cerr << "[spawnObject] Calling onNetworkSpawn (second)...\n";
    identity->onNetworkSpawn();

    std::cerr << "[spawnObject] Checking server for broadcast...\n";
    if (server)
    {
        std::cerr << "[spawnObject] Creating spawn message...\n";
        const SpawnMessage message = createSpawnMessage(identity, assetId);
        std::cerr << "[spawnObject] Broadcasting...\n";
        server->broadcastMessage(message);
    }

    std::cerr << "[spawnObject] Done!\n";
    return gameObjectPointer;
}

GameObject* NetworkSpawnManager::spawnPlayer(int clientId,
                                             Vector2 spawnPosition)
{
	constexpr uint32_t PLAYER_ASSET_ID = 1;
	return spawnObject(PLAYER_ASSET_ID, spawnPosition, clientId);
}

void NetworkSpawnManager::despawnObject(uint32_t netId)
{
	auto it = spawnedObjects.find(netId);
	if (it == spawnedObjects.end())
	{
		return;
	}

	GameObject* object = it->second;

	auto* identity = object->getComponent<NetworkIdentity>();
	if (identity)
	{
		identity->onNetworkDespawn();

		int ownerId = identity->getOwnerId();
		if (ownerId >= 0)
		{
			auto& owned = clientOwnedObjects[ownerId];
			owned.erase(std::ranges::remove(owned, netId).begin(),
			            owned.end());
		}
	}

	if (server)
	{
		ObjectDestroyMessage msg;
		msg.netId = netId;
		server->broadcastMessage(msg);
	}

	spawnedObjects.erase(netId);
	objectAssets.erase(netId);

	object->destroy();
}

void NetworkSpawnManager::despawnClientObjects(int clientId)
{
	auto it = clientOwnedObjects.find(clientId);
	if (it == clientOwnedObjects.end())
	{
		return;
	}

	std::vector<uint32_t> toRemove = it->second;

	for (uint32_t netId : toRemove)
	{
		despawnObject(netId);
	}

	clientOwnedObjects.erase(clientId);
}

void NetworkSpawnManager::syncExistingObjects(int clientId)
{
	if (!server) return;

	for (const auto& [networkIdentity, gameObject] : spawnedObjects)
	{
		auto* identity = gameObject->getComponent<NetworkIdentity>();
		if (!identity) continue;

		const uint32_t assetId = objectAssets[networkIdentity];
		SpawnMessage message = createSpawnMessage(identity, assetId);
		server->sendMessage(clientId, message);
	}
}

GameObject* NetworkSpawnManager::findByNetId(uint32_t netId) const
{
	auto it = spawnedObjects.find(netId);
	return (it != spawnedObjects.end()) ? it->second : nullptr;
}

SpawnMessage NetworkSpawnManager::createSpawnMessage(
	const NetworkIdentity* identity, uint32_t assetId)
{
	SpawnMessage message;
	message.netId = identity->getNetId();
	message.assetId = assetId;
	message.ownerId = identity->getOwnerId();

	const Transform* transform = identity->getTransform();
	if (transform)
	{
		message.position = transform->getPosition();
		message.rotation = transform->getRotationAngle();
		message.scale = transform->getScale();
	}

	auto clone = prefabLibrary->instantiate(assetId);
	if (clone)
	{
		auto* cloneIdentity = clone->getComponent<NetworkIdentity>();
		if (cloneIdentity)
		{
			cloneIdentity->networkId = message.netId;
			cloneIdentity->ownerId = message.ownerId;
		}

		auto* cloneTransform = clone->getTransform();
		if (cloneTransform)
		{
			cloneTransform->setPosition(message.position);
			cloneTransform->setRotationAngle(message.rotation);
			cloneTransform->setScale(message.scale);
		}

		message.gameObject = std::move(clone);
	}

	return message;
}

void NetworkSpawnManager::handleSpawnMessage(SpawnMessage& message)
{
	if (!message.gameObject)
	{
		return;
	}

	if (!getScene())
	{
		return;
	}

	auto* identity = message.gameObject->getComponent<NetworkIdentity>();
	if (!identity)
	{
		identity = message.gameObject->addComponent<NetworkIdentity>();
	}

	identity->networkId = message.netId;
	identity->ownerId = message.ownerId;
	identity->gameWorld = gameWorld;

	if (identityRegistry)
	{
		identityRegistry->registerIdentity(identity);
	}

	GameObject* rawPtr = message.gameObject.get();
	spawnedObjects[message.netId] = rawPtr;
	objectAssets[message.netId] = message.assetId;

	getScene()->addGameObject(std::move(message.gameObject));
	identity->onNetworkSpawn();
}

uint32_t NetworkSpawnManager::addToPrefabLibrary(std::unique_ptr<GameObject> gameObject)
{
	if (prefabLibrary == nullptr)
	{
		return -1;
	}
	return prefabLibrary->add( std::move(gameObject));
}


GameObject* NetworkSpawnManager::getObjectByNetId(uint32_t netId) const
{
	auto it = spawnedObjects.find(netId);
	if (it != spawnedObjects.end())
	{
		return it->second;
	}
	return nullptr;
}

NetworkIdentityRegistry& NetworkSpawnManager::getNetworkIdentityRegistry()
{
	return *identityRegistry;
}

Scene* NetworkSpawnManager::getScene() const
{
	if (gameWorld && gameWorld->sceneManager)
	{
		return gameWorld->sceneManager->getActiveScene();
	}
	return nullptr;
}