#pragma once

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <vector>

#include "NetworkingIdentityRegistry.h"
#include "Core/GameWorld.h"
#include "Math/Vector2.h"
#include "Networking/PrefabLibrary.h"

class GameObject;
class Scene;
class Server;
class NetworkIdentity;
struct SpawnMessage;

/**
 * @brief Manages spawning and despawning of networked objects.
 *
 * Server-authoritative: only server creates network IDs and broadcasts spawns.
 * Clients receive SpawnMessages and create local representations.
 */
class NetworkSpawnManager
{
public:
	/**
	 * @brief Constructs a NetworkSpawnManager.
	 *
	 * @param server Server instance for broadcasting (nullptr on client)
	 * @param scene Scene to spawn objects into
	 * @param registry Registry for tracking NetworkIdentities
	 * @param prefabLibrary Library for instantiating prefabs
	 */
	NetworkSpawnManager(GameWorld* gameWorlds);

	/**
	 * @brief Spawns a networked object (server-side).
	 *
	 * @param assetId Prefab identifier from PrefabLibrary
	 * @param ownerId Client that owns this object (-1 for server)
	 * @return Pointer to spawned object, or nullptr on failure
	 */
	GameObject* spawnObject(uint32_t assetId,
                            int ownerId = -1);

	/**
	 * @brief Destroys a networked object and notifies clients.
	 */
	void despawnObject(uint32_t netId);

	/**
	 * @brief Despawns all objects owned by a client (on disconnect).
	 */
	void despawnClientObjects(int clientId);

	/**
	 * @brief Sends all existing spawned objects to a newly connected client.
	 */
	void syncExistingObjects(int clientId);

	/**
	 * @brief Finds a networked object by its network ID.
	 */
	GameObject* findByNetId(uint32_t netId) const;

	/**
	 * @brief Client-side: handles incoming SpawnMessage.
	 */
	void handleSpawnMessage(SpawnMessage& message);

    void CheckNewClientSpawnObject(int clientId);

	uint32_t addToPrefabLibrary(
		std::unique_ptr<GameObject> gameObject);

	GameObject* getObjectByNetId(uint32_t netId) const;

	NetworkIdentityRegistry& getNetworkIdentityRegistry();
	Scene* getScene() const;
    void trackSpawnedObject(uint32_t netId, GameObject* object);
    void untrackSpawnedObject(uint32_t netId);

	void loadAssetsForGameObject(GameObject *obj);
private:
	SpawnMessage createSpawnMessage(const NetworkIdentity* identity,
	                                uint32_t assetId);

	uint32_t generateNetId();


	GameWorld* gameWorld;
	std::unique_ptr<NetworkIdentityRegistry> identityRegistry;
	std::unique_ptr<PrefabLibrary> prefabLibrary;

	uint32_t nextNetworkId = 1;

	/// netId -> GameObject mapping
	std::unordered_map<uint32_t, GameObject*> spawnedObjects;

	/// netId -> assetId (for syncing to new clients)
	std::unordered_map<uint32_t, uint32_t> objectAssets;

	/// clientId -> list of owned netIds
	std::unordered_map<int, std::vector<uint32_t>> clientOwnedObjects;
};