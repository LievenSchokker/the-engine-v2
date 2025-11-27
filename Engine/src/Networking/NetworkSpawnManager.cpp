#include "Networking/NetworkSpawnManager.h"
#include "Networking/NetworkIdentity.h"
#include "Networking/NetworkPrefabRegistry.h"
#include "Networking/Server/Server.h"
#include "Networking/Messages/Concretes/SpawnMessage.h"
#include "Networking/Messages/Concretes/ObjectDestroyMessage.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Component/Transform.h"

#include <iostream>

NetworkSpawnManager::NetworkSpawnManager(Server* server, Scene* scene, NetworkIdentityRegistry* registry)
    : server(server)
    , scene(scene)
    , identityRegistry(registry)
{
}

uint32_t NetworkSpawnManager::generateNetId()
{
    return nextNetId++;
}

GameObject* NetworkSpawnManager::spawnObject(uint32_t assetId, Vector2 position, int ownerId)
{
    std::cout << "spawnObject called, assetId=" << assetId << std::endl;

    auto gameObject = NetworkPrefabRegistry::instance().create(assetId);
    if (!gameObject)
    {
        std::cerr << "Unknown assetId: " << assetId << std::endl;
        return nullptr;
    }

    std::cout << "Prefab created" << std::endl;

    gameObject->getTransform()->setPosition(position);

    std::cout << "Position set" << std::endl;

    auto* identity = gameObject->getComponent<NetworkIdentity>();
    if (!identity)
    {
        identity = gameObject->addComponent<NetworkIdentity>();
    }

    std::cout << "NetworkIdentity ready" << std::endl;

    // Assign netId FIRST
    uint32_t netId = generateNetId();
    identity->netId = netId;
    identity->ownerId = ownerId;

    std::cout << "Assigned netId=" << netId << std::endl;

    // Register AFTER netId is set
    if (identityRegistry && identity)
    {
        identityRegistry->registerIdentity(identity);
    }

    GameObject* rawPtr = gameObject.get();
    spawnedObjects[netId] = rawPtr;
    objectAssets[netId] = assetId;

    if (ownerId >= 0)
    {
        clientOwnedObjects[ownerId].push_back(netId);
    }

    std::cout << "Adding to scene..." << std::endl;

    scene->addGameObject(std::move(gameObject));

    std::cout << "Added to scene, calling onNetworkSpawn..." << std::endl;

    identity->onNetworkSpawn();

    std::cout << "onNetworkSpawn done, broadcasting..." << std::endl;

    if (server)
    {
        SpawnMessage msg = createSpawnMessage(identity, assetId);
        std::cout << "SpawnMessage created, sending..." << std::endl;
        bool sent = server->broadcastMessage(msg);
        std::cout << "Broadcast result: " << (sent ? "success" : "failed") << std::endl;
    }

    std::cout << "Spawned object netId=" << netId
              << " assetId=" << assetId << " owner=" << ownerId << std::endl;

    return rawPtr;
}

GameObject* NetworkSpawnManager::spawnPlayer(int clientId, Vector2 spawnPosition)
{
    // Asset ID 1 = Player prefab (convention)
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

    GameObject* obj = it->second;

    // Get owner before removing
    auto* identity = obj->getComponent<NetworkIdentity>();
    if (identity)
    {
        identity->onNetworkDespawn();

        int ownerId = identity->getOwnerId();
        if (ownerId >= 0)
        {
            auto& owned = clientOwnedObjects[ownerId];
            owned.erase(std::remove(owned.begin(), owned.end(), netId), owned.end());
        }
    }

    // Broadcast despawn to clients
    if (server)
    {
        ObjectDestroyMessage msg;
        msg.netId = netId;
        server->broadcastMessage(msg);
    }

    // Remove from tracking
    spawnedObjects.erase(netId);
    objectAssets.erase(netId);

    // Destroy the object
    obj->destroy();

    std::cout << "Despawned object netId=" << netId << std::endl;
}

void NetworkSpawnManager::despawnClientObjects(int clientId)
{
    auto it = clientOwnedObjects.find(clientId);
    if (it == clientOwnedObjects.end())
    {
        return;
    }

    // Copy the list since despawnObject modifies it
    std::vector<uint32_t> toRemove = it->second;

    for (uint32_t netId : toRemove)
    {
        despawnObject(netId);
    }

    clientOwnedObjects.erase(clientId);

    std::cout << "Despawned all objects for client " << clientId << std::endl;
}

void NetworkSpawnManager::syncExistingObjects(int clientId)
{
    if (!server) return;

    for (const auto& [netId, obj] : spawnedObjects)
    {
        auto* identity = obj->getComponent<NetworkIdentity>();
        if (!identity) continue;

        uint32_t assetId = objectAssets[netId];
        SpawnMessage msg = createSpawnMessage(identity, assetId);
        server->sendMessage(clientId, msg);
    }

    std::cout << "Synced " << spawnedObjects.size()
              << " objects to client " << clientId << std::endl;
}

GameObject* NetworkSpawnManager::findByNetId(uint32_t netId) const
{
    auto it = spawnedObjects.find(netId);
    return (it != spawnedObjects.end()) ? it->second : nullptr;
}

SpawnMessage NetworkSpawnManager::createSpawnMessage(NetworkIdentity* identity, uint32_t assetId)
{
    SpawnMessage msg;
    msg.netId = identity->getNetId();
    msg.assetId = assetId;
    msg.ownerId = identity->getOwnerId();

    const Transform* transform = identity->getTransform();
    if (transform)
    {
        msg.position = transform->getPosition();
        msg.rotation = transform->getRotationAngle();
        msg.scale = transform->getScale();
    }

    return msg;
}

void NetworkSpawnManager::handleSpawnMessage(const SpawnMessage& message)
{
    // Client-side: create object from spawn message
    if (spawnedObjects.contains(message.netId))
    {
        // Already exists
        return;
    }

    auto gameObject = NetworkPrefabRegistry::instance().create(message.assetId);
    if (!gameObject)
    {
        std::cerr << "Client: Unknown assetId " << message.assetId << std::endl;
        return;
    }

    // Apply transform
    gameObject->getTransform()->setPosition(message.position);
    gameObject->getTransform()->setRotationAngle(message.rotation);
    gameObject->getTransform()->setScale(message.scale);

    // Setup NetworkIdentity
    auto* identity = gameObject->getComponent<NetworkIdentity>();
    if (!identity)
    {
        identity = gameObject->addComponent<NetworkIdentity>();
    }
    identity->netId = message.netId;
    identity->ownerId = message.ownerId;

    // Register with identity registry
    if (identityRegistry && identity)
    {
        identityRegistry->registerIdentity(identity);
    }

    // Track
    GameObject* rawPtr = gameObject.get();
    spawnedObjects[message.netId] = rawPtr;
    objectAssets[message.netId] = message.assetId;

    // Add to scene
    scene->addGameObject(std::move(gameObject));

    // Initialize
    identity->onNetworkSpawn();

    std::cout << "Client spawned netId=" << message.netId << std::endl;
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