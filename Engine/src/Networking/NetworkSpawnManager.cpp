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

#include "Assets/SpritesheetLoader.h"
#include "Component/SpriteComponent.h"

NetworkSpawnManager::NetworkSpawnManager(GameWorld* gameWorlds) :
       identityRegistry(std::make_unique<NetworkIdentityRegistry>())
     , prefabLibrary(std::make_unique<PrefabLibrary>())
     , gameWorld(gameWorlds)
{

}

NetworkSpawnManager::~NetworkSpawnManager()
{
	if (gameWorld != nullptr && gameWorld->server != nullptr)
	{
		gameWorld->server->setClientDisconnectedCallback(nullptr);
	}
}

uint32_t NetworkSpawnManager::generateNetId()
{
	return nextNetworkId++;
}

void NetworkSpawnManager::clearPrefabLibrary()
{
	if (prefabLibrary)
	{
		prefabLibrary->clear();
	}
}

GameObject* NetworkSpawnManager::spawnObject(const uint32_t assetId, const int ownerId)
{
    if (!prefabLibrary)
    {
        return nullptr;
    }

    auto gameObject = prefabLibrary->instantiate(assetId);

    if (!gameObject)
    {
        return nullptr;
    }

	gameObject->setName(gameObject->getName() + std::to_string(ownerId));

    auto* identity = gameObject->getComponent<NetworkIdentity>();
    if (!identity)
    {
        identity = gameObject->addComponent<NetworkIdentity>();
    }

    const uint32_t networkId = generateNetId();

    identity->networkId = networkId;
    identity->ownerId = ownerId;
    identity->gameWorld = gameWorld;

    identity->onNetworkSpawn();

    if (identityRegistry)
    {
        identityRegistry->registerIdentity(identity);
    }

    GameObject* gameObjectPointer = gameObject.get();
    spawnedObjects[networkId] = gameObjectPointer;
    objectAssets[networkId] = assetId;

    if (ownerId >= 0)
    {
        clientOwnedObjects[ownerId].push_back(networkId);
    }

    if (!getScene())
    {
        return nullptr;
    }

	gameWorld->sceneManager->getActiveScene()->addGameObject(std::move(gameObject));

    if (gameWorld->isServer())
    {
        identity->onNetworkInstantiate(nextNetworkId);
    }

    identity->onNetworkSpawn();

    if (gameWorld->isServer())
    {
        const SpawnMessage message = createSpawnMessage(identity, assetId);
        gameWorld->server->broadcastMessage(message);
    }

    return gameObjectPointer;
}

void NetworkSpawnManager::despawnObject(uint32_t netId)
{
	auto gameObjectEntry = spawnedObjects.find(netId);
	if (gameObjectEntry == spawnedObjects.end())
	{
		return;
	}

	GameObject* object = gameObjectEntry->second;

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

	if (gameWorld->server)
	{
		ObjectDestroyMessage message;
		message.netId = netId;
		gameWorld->server->broadcastMessage(message);
	}

	spawnedObjects.erase(netId);
	objectAssets.erase(netId);
    identityRegistry->unregisterIdentity(identity);

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
	if (!gameWorld->server) return;

	for (const auto& [networkIdentity, gameObject] : spawnedObjects)
	{
		auto* identity = gameObject->getComponent<NetworkIdentity>();
		if (!identity) continue;

		const uint32_t assetId = objectAssets[networkIdentity];
		SpawnMessage message = createSpawnMessage(identity, assetId);
		gameWorld->server->sendMessage(clientId, message);
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

	auto clone = prefabLibrary->instantiate(assetId);
	if (clone)
	{
		auto* cloneIdentity = clone->getComponent<NetworkIdentity>();
		if (cloneIdentity)
		{
			cloneIdentity->networkId = message.netId;
			cloneIdentity->ownerId = message.ownerId;
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


    if (spawnedObjects.contains(message.netId))
    {
        if (GameObject* existing = spawnedObjects[message.netId])
        {
            existing->copyStateFrom(*message.gameObject);
        }

        return;
    }

	if (!identity)
	{
		identity = message.gameObject->addComponent<NetworkIdentity>();
	}

	identity->networkId = message.netId;
	identity->ownerId = message.ownerId;
	identity->gameWorld = gameWorld;

	message.gameObject->setName(message.gameObject->getName() + std::to_string(message.ownerId));

	if (identityRegistry)
	{
		identityRegistry->registerIdentity(identity);
	}

	GameObject* rawPtr = message.gameObject.get();

	if (gameWorld->isClient())
	{
		loadAssetsForGameObject(rawPtr);
	}

	spawnedObjects[message.netId] = rawPtr;
	objectAssets[message.netId] = message.assetId;
	gameWorld->sceneManager->getActiveScene()->addGameObject(std::move(message.gameObject));
	identity->onNetworkSpawn();
}

void NetworkSpawnManager::CheckNewClientSpawnObject(int clientId)
{
    std::vector<uint32_t> ids = prefabLibrary->getNetworkPrefabIdsWithAuthoritativeClient();

    for (const auto id : ids)
    {
        GameObject* spawned = spawnObject(id, clientId);

        if (spawned)
        {
            auto* identity = spawned->getComponent<NetworkIdentity>();
        }
    }
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

void NetworkSpawnManager::trackSpawnedObject(uint32_t netId, GameObject* object)
{
    spawnedObjects[netId] = object;
}

void NetworkSpawnManager::untrackSpawnedObject(uint32_t netId)
{
    spawnedObjects.erase(netId);
}

void NetworkSpawnManager::loadAssetsForGameObject(GameObject* obj)
{
	if (!obj || !gameWorld) return;

	AssetManager* assetManager = gameWorld->getAssetManager();
	if (!assetManager) return;

	for (auto& component : obj->getComponents())
	{
		// #TODO MAKE A GENERIC ASSETCOMPONENT
		if (auto* sprite = dynamic_cast<SpriteComponent*>(component.get()))
		{
			// Skip if already loaded
			if (sprite->getSprite() != nullptr && sprite->getSprite()->isLoaded())
			{
				continue;
			}

			std::string path = sprite->getPath();
			if (path.empty())
			{
				continue;
			}

			SpritesheetLoader::loadSpritesheet(
				assetManager,
				sprite,
				path,
				sprite->getSpritesheetDefinition()
			);
		}
	}
}

void NetworkSpawnManager::configureServerCallbacks(Server* server)
{
	if (!server) return;

	server->setClientDisconnectedCallback([this](int clientId) {
		despawnClientObjects(clientId);
	});
}