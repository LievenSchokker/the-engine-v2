#pragma once

#include <cstdint>
#include <unordered_map>
#include <memory>
#include "GameObject/Vector2.h"

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
    explicit NetworkSpawnManager(Server* server, Scene* scene);

    /**
     * @brief Spawns a networked object (server-side).
     *
     * @param assetId Prefab identifier from NetworkPrefabRegistry
     * @param position Initial world position
     * @param ownerId Client that owns this object (-1 for server)
     * @return Pointer to spawned object, or nullptr on failure
     */
    GameObject* spawnObject(uint32_t assetId, Vector2 position, int ownerId = -1);

    /**
     * @brief Spawns a player for a newly connected client.
     */
    GameObject* spawnPlayer(int clientId, Vector2 spawnPosition);

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
    void handleSpawnMessage(const SpawnMessage& message);

private:
    uint32_t generateNetId();
    SpawnMessage createSpawnMessage(NetworkIdentity* identity, uint32_t assetId);

    Server* server;
    Scene* scene;
    uint32_t nextNetId = 1;

    // netId -> GameObject mapping
    std::unordered_map<uint32_t, GameObject*> spawnedObjects;

    // netId -> assetId (for syncing to new clients)
    std::unordered_map<uint32_t, uint32_t> objectAssets;

    // clientId -> list of owned netIds
    std::unordered_map<int, std::vector<uint32_t>> clientOwnedObjects;
};