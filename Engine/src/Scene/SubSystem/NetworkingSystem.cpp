#include "Scene/SubSystems/NetworkSystem.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Component/Transform.h"
#include "Component/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkSpawnManager.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/IPhysicsWorld.h"
#include "Assets/SpritesheetLoader.h"
#include "Component/SpriteComponent.h"
#include "Scene/SubSystems/BehaviourSystem.h"

#include <iostream>

void NetworkSystem::configure(NetworkSpawnManager* spawnMgr, BehaviourSystem* behSys)
{
    spawnManager = spawnMgr;
    behaviourSystem = behSys;
    configured = (spawnManager != nullptr);
}

bool NetworkSystem::isConfigured() const
{
    return configured;
}

void NetworkSystem::processSceneForNetwork(Scene& scene, GameWorld& gameWorld)
{
    if (!configured) return;

    std::cout << "[NetworkSystem] Processing scene: " << scene.getName() << std::endl;

    if (gameWorld.server != nullptr)
    {
        std::cout << "[NetworkSystem] Processing as SERVER" << std::endl;
        processForServer(scene, gameWorld);
    }
    else
    {
        std::cout << "[NetworkSystem] Processing as CLIENT" << std::endl;
        processForClient(scene);
    }
}

void NetworkSystem::processForServer(Scene& scene, GameWorld& gameWorld)
{
    std::vector<GameObject*> networkObjects;

    scene.forEachGameObject([&](GameObject& obj) {
        if (hasNetworkBehaviour(obj) && !hasNetworkIdentity(obj))
        {
            networkObjects.push_back(&obj);
        }
    });

    std::cout << "[NetworkSystem] Found " << networkObjects.size()
              << " network objects to process" << std::endl;

    if (networkObjects.empty()) return;

    for (const GameObject* obj : networkObjects)
    {
        auto behaviour = obj->getComponent<NetworkBehaviour>();

        if (behaviour && behaviour->getAuthorityType() == AuthorityType::ClientAuthority)
        {
            // ClientAuthority: just register as prefab
            auto extracted = scene.extractGameObject(obj->getGameObjectHandle());
            if (!extracted) continue;

            spawnManager->addToPrefabLibrary(std::move(extracted));
            continue;
        }

        // ServerAuthority: register as prefab AND spawn
        Vector2 spawnPosition = obj->getTransform()->getPosition();
        auto extracted = scene.extractGameObject(obj->getGameObjectHandle());
        if (!extracted) continue;

        extracted->getTransform()->setPosition(spawnPosition);

        if (!extracted->getComponent<NetworkIdentity>())
        {
            extracted->addComponent<NetworkIdentity>();
        }

        uint32_t assetId = spawnManager->addToPrefabLibrary(std::move(extracted));
        spawnManager->spawnObject(assetId, -1);
    }
}

void NetworkSystem::processForClient(Scene& scene)
{
    std::vector<GameObject*> toProcess;

    scene.forEachGameObject([&](GameObject& obj) {
        if (hasNetworkBehaviour(obj) && !hasNetworkIdentity(obj))
        {
            toProcess.push_back(&obj);
        }
    });

    if (toProcess.empty()) return;

    for (const GameObject* obj : toProcess)
    {
        auto extracted = scene.extractGameObject(obj->getGameObjectHandle());
        if (!extracted) continue;

        extracted->getTransform()->setPosition({0, 0});

        if (!extracted->getComponent<NetworkIdentity>())
        {
            extracted->addComponent<NetworkIdentity>();
        }

        spawnManager->addToPrefabLibrary(std::move(extracted));
    }
}

void NetworkSystem::applyNetworkSnapshot(
    const std::string& targetSceneName,
    std::vector<std::unique_ptr<GameObject>>& receivedObjects,
    Scene*& activeScene,
    GameWorld& gameWorld)
{
    if (!spawnManager || !activeScene) return;

    // CRITICAL FIX: Capture the scene pointer at the START
    // Don't use activeScene directly in the loop as it may change
    Scene* targetScene = activeScene;

    // Verify we're spawning into the correct scene
    if (targetScene->getName() != targetSceneName)
    {
        std::cerr << "[NetworkSystem] ERROR: Scene mismatch! Target: " << targetSceneName
                  << " but activeScene is: " << targetScene->getName() << std::endl;
        return;  // Don't spawn into wrong scene
    }

    bool shouldLog = (frameCount++ % 60 == 0);

    if (shouldLog)
    {
        std::cout << "[NetworkSystem] Snapshot for " << targetSceneName
                  << ", objects: " << receivedObjects.size() << std::endl;
    }

    std::unordered_set<uint32_t> receivedNetIds;
    std::vector<std::pair<GameObject*, uint32_t>> toFixup;

    for (auto& received : receivedObjects)
    {
        if (!received) continue;

        auto* identity = received->getComponent<NetworkIdentity>();
        if (!identity) continue;

        uint32_t netId = identity->getNetId();
        receivedNetIds.insert(netId);

        GameObject* existing = spawnManager->getObjectByNetId(netId);

        if (existing)
        {
            updateExistingObject(existing, *received, gameWorld, toFixup, *targetScene);
        }
        else
        {
            spawnNewObject(std::move(received), gameWorld, toFixup, *targetScene);
        }
    }

    // Fix up parent relationships
    for (auto& [child, parentNetId] : toFixup)
    {
        if (GameObject* parent = spawnManager->getObjectByNetId(parentNetId))
        {
            child->setParent(parent);
        }
    }

    // Remove objects not in snapshot
    removeStaleObjects(receivedNetIds, gameWorld, *targetScene);

    // Apply physics snapshot
    if (gameWorld.physics)
    {
        gameWorld.physics->applyNetworkSnapshot();
    }
}

void NetworkSystem::updateExistingObject(
    GameObject* existing,
    GameObject& received,
    GameWorld& gameWorld,
    std::vector<std::pair<GameObject*, uint32_t>>& toFixup,
    Scene& activeScene)
{
    existing->copyStateFrom(received);

    // Sync physics
    if (gameWorld.physics)
    {
        auto* rigidBody = existing->getComponent<RigidBody>();
        auto* transform = existing->getTransform();

        if (rigidBody && transform)
        {
            gameWorld.physics->setBodyTransform(
                rigidBody,
                transform->getPosition(),
                transform->getRotationAngle()
            );

            // Zero velocity for non-owned objects on client
            if (gameWorld.isClient() && !gameWorld.isServer())
            {
                auto* identity = existing->getComponent<NetworkIdentity>();
                if (identity)
                {
                    int ownerId = identity->getOwnerId();
                    if (ownerId < 0 || ownerId != gameWorld.localClientId)
                    {
                        gameWorld.physics->setLinearVelocity(rigidBody, {0.0f, 0.0f});
                    }
                }
            }
        }
    }

    // Handle parent fixup
    if (auto parentNetId = existing->consumePendingParentNetId())
    {
        toFixup.push_back({existing, *parentNetId});
    }

    // Handle inline children
    for (auto& child : existing->consumeInlineChildren())
    {
        GameObject* childPtr = child.get();
        activeScene.addGameObject(std::move(child));
        addInlineChildrenRecursive(childPtr, activeScene);
    }

    // Reload assets if needed
    if (gameWorld.getAssetManager())
    {
        reloadAssetsForGameObject(existing, *gameWorld.getAssetManager());
    }
}

void NetworkSystem::spawnNewObject(
    std::unique_ptr<GameObject> received,
    GameWorld& gameWorld,
    std::vector<std::pair<GameObject*, uint32_t>>& toFixup,
    Scene& activeScene)
{
    auto* identity = received->getComponent<NetworkIdentity>();
    uint32_t netId = identity ? identity->getNetId() : 0;

    std::cout << "[NetworkSystem] SPAWN: " << received->getName()
              << " netId=" << netId << std::endl;

    if (identity)
    {
        identity->setWorld(&gameWorld);
        spawnManager->getNetworkIdentityRegistry().registerIdentity(identity);
    }

    std::vector<Behaviour*> behaviours = received->getAllBehaviours();
    GameObject* rawPtr = received.get();

    if (auto parentNetId = received->consumePendingParentNetId())
    {
        toFixup.push_back({rawPtr, *parentNetId});
    }

    activeScene.addGameObject(std::move(received));
    spawnManager->trackSpawnedObject(netId, rawPtr);

    if (behaviourSystem)
    {
        behaviourSystem->initialiseRuntimeBehaviours(behaviours, gameWorld);
    }

    if (gameWorld.getAssetManager())
    {
        reloadAssetsForGameObject(rawPtr, *gameWorld.getAssetManager());
    }

    if (identity)
    {
        identity->onNetworkSpawn();
    }

    addInlineChildrenRecursive(rawPtr, activeScene);
}

void NetworkSystem::removeStaleObjects(
    const std::unordered_set<uint32_t>& receivedNetIds,
    GameWorld& gameWorld,
    Scene& activeScene)
{
    std::vector<uint32_t> toRemove;

    // Copy the list first to avoid iterator invalidation
    auto allIdentities = spawnManager->getNetworkIdentityRegistry().getAllIdentities();

    for (auto* identity : allIdentities)
    {
        if (!identity) continue;

        uint32_t netId = identity->getNetId();

        if (isLocallyOwned(identity, gameWorld)) continue;

        if (!receivedNetIds.contains(netId))
        {
            toRemove.push_back(netId);
        }
    }

    for (uint32_t netId : toRemove)
    {
        // Get object directly from scene, not from spawnManager
        NetworkIdentity* identity = spawnManager->getNetworkIdentityRegistry().findByNetId(netId);
        if (!identity) continue;

        GameObject* object = identity->getGameObject();
        if (!object) continue;

        std::cout << "[NetworkSystem] Removing stale object: " << object->getName()
                  << " netId=" << netId << std::endl;

        identity->onNetworkDespawn();
        spawnManager->getNetworkIdentityRegistry().unregisterIdentity(identity);
        spawnManager->untrackSpawnedObject(netId);

        activeScene.removeGameObject(object->getGameObjectHandle());
    }
}

void NetworkSystem::cleanupNetworkObjectsInScene(Scene& scene)
{
    if (!spawnManager) return;

    auto& registry = spawnManager->getNetworkIdentityRegistry();

    std::vector<std::pair<uint32_t, ObjectHandle>> toCleanup;

    scene.forEachGameObject([&](GameObject& obj) {
        auto* identity = obj.getComponent<NetworkIdentity>();
        if (identity && identity->getNetId() != 0)
        {
            std::cout << "[NetworkSystem] Will cleanup: " << obj.getName()
                      << " netId=" << identity->getNetId() << std::endl;
            toCleanup.push_back({identity->getNetId(), obj.getGameObjectHandle()});
        }
    });

    std::cout << "[NetworkSystem] Cleaning up " << toCleanup.size()
              << " network objects in scene: " << scene.getName() << std::endl;

    for (auto& [netId, handle] : toCleanup)
    {
        GameObject* obj = scene.getGameObject(handle);
        if (!obj)
        {
            std::cout << "[NetworkSystem] WARNING: Object already gone for netId=" << netId << std::endl;
            continue;
        }

        std::cout << "[NetworkSystem] Removing: " << obj->getName() << " netId=" << netId << std::endl;

        auto* identity = obj->getComponent<NetworkIdentity>();
        if (identity)
        {
            identity->onNetworkDespawn();
            registry.unregisterIdentity(identity);
        }
        spawnManager->untrackSpawnedObject(netId);

        bool removed = scene.removeGameObject(handle);
        std::cout << "[NetworkSystem] Remove result: " << (removed ? "success" : "FAILED") << std::endl;
    }

    // ADD THIS: Verify scene is actually empty of network objects
    int remaining = 0;
    scene.forEachGameObject([&](GameObject& obj) {
        if (obj.getComponent<NetworkIdentity>()) {
            remaining++;
            std::cout << "[NetworkSystem] STILL IN SCENE: " << obj.getName() << std::endl;
        }
    });
    std::cout << "[NetworkSystem] Remaining network objects after cleanup: " << remaining << std::endl;
}

bool NetworkSystem::isLocallyOwned(NetworkIdentity* identity, const GameWorld& gameWorld) const
{
    if (!identity) return false;

    GameObject* go = identity->getGameObject();
    if (!go) return false;

    for (Behaviour* behaviour : go->getAllBehaviours())
    {
        auto* netBehaviour = dynamic_cast<NetworkBehaviour*>(behaviour);
        if (!netBehaviour) continue;

        if (netBehaviour->getAuthorityType() == AuthorityType::ClientAuthority &&
            identity->getOwnerId() == gameWorld.localClientId)
        {
            return true;
        }
    }

    return false;
}

void NetworkSystem::reloadAssetsForGameObject(GameObject* obj, AssetManager& assetManager)
{
    for (auto& component : obj->getComponents())
    {
        if (auto* sprite = dynamic_cast<SpriteComponent*>(component.get()))
        {
            if (sprite->getSprite() && sprite->getSprite()->isLoaded()) continue;

            std::string path = sprite->getPath();
            if (path.empty()) continue;

            SpritesheetLoader::loadSpritesheet(
                &assetManager, sprite, path, sprite->getSpritesheetDefinition()
            );
        }
    }
}

void NetworkSystem::addInlineChildrenRecursive(GameObject* obj, Scene& activeScene)
{
    for (auto& child : obj->consumeInlineChildren())
    {
        GameObject* childPtr = child.get();
        activeScene.addGameObject(std::move(child));
        addInlineChildrenRecursive(childPtr, activeScene);
    }
}

bool NetworkSystem::hasNetworkBehaviour(const GameObject& obj) const
{
    for (const auto& component : obj.getComponents())
    {
        if (dynamic_cast<NetworkBehaviour*>(component.get())) return true;
    }
    return false;
}

bool NetworkSystem::hasNetworkIdentity(const GameObject& obj) const
{
    return obj.getComponent<NetworkIdentity>() != nullptr;
}