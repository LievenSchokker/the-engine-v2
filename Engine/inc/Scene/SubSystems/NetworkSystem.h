#pragma once

#include <vector>
#include <memory>
#include <unordered_set>
#include <string>

class Scene;
class GameObject;
class GameWorld;
class NetworkSpawnManager;
class NetworkIdentity;
class BehaviourSystem;
class AssetManager;

/**
 * @brief Handles network synchronization of GameObjects across scenes.
 *
 * NetworkSystem is responsible for:
 * - Processing scenes when they become active (extracting networked objects as prefabs)
 * - Applying network snapshots received from the server
 * - Cleaning up networked objects during scene transitions
 * - Tracking object ownership for client authority
 *
 * On the server, networked objects (those with NetworkBehaviour) are extracted
 * from scenes and registered as prefabs, then spawned through NetworkSpawnManager.
 * On clients, the same extraction occurs but objects are only spawned when
 * snapshots arrive from the server.
 *
 * @see NetworkSpawnManager
 * @see NetworkIdentity
 * @see NetworkBehaviour
 */
class NetworkSystem
{
public:
    NetworkSystem() = default;
    ~NetworkSystem() = default;

    /**
     * @brief Configures the NetworkSystem with required dependencies.
     *
     * Must be called before any other methods. The system is considered
     * configured when a valid NetworkSpawnManager is provided.
     *
     * @param spawnManager Pointer to the spawn manager for object instantiation.
     * @param behaviourSystem Pointer to the behaviour system for initializing behaviours.
     */
    void configure(NetworkSpawnManager* spawnManager, BehaviourSystem* behaviourSystem);

    /**
     * @brief Checks if the system has been configured.
     *
     * @return true if configure() was called with a valid NetworkSpawnManager.
     */
    [[nodiscard]] bool isConfigured() const;

    /**
     * @brief Processes a scene for network synchronization.
     *
     * Called when a scene becomes active. Extracts GameObjects with
     * NetworkBehaviour components and registers them as network prefabs.
     *
     * Server behavior:
     * - ServerAuthority objects: registered as prefabs and immediately spawned
     * - ClientAuthority objects: registered as prefabs only (spawned when clients connect)
     *
     * Client behavior:
     * - All networked objects are extracted and registered as prefabs
     * - Actual spawning occurs when snapshots arrive from the server
     *
     * @param scene The scene to process.
     * @param gameWorld The game world context.
     */
    void processSceneForNetwork(Scene& scene, GameWorld& gameWorld);

    /**
     * @brief Applies a network state snapshot received from the server.
     *
     * Synchronizes the local scene state with the server's authoritative state.
     * This includes:
     * - Updating existing objects with new state
     * - Spawning new objects that don't exist locally
     * - Removing stale objects that no longer exist on the server
     * - Fixing up parent-child relationships
     *
     * @param targetSceneName Expected scene name (must match active scene).
     * @param receivedObjects Objects received in the snapshot (ownership transferred).
     * @param activeScene Reference to the active scene pointer.
     * @param gameWorld The game world context.
     *
     * @note If targetSceneName doesn't match the active scene, the snapshot is rejected
     *       to prevent spawning objects into the wrong scene.
     */
    void applyNetworkSnapshot(
        const std::string& targetSceneName,
        std::vector<std::unique_ptr<GameObject>>& receivedObjects,
        Scene*& activeScene,
        GameWorld& gameWorld
    );

    /**
     * @brief Checks if a networked object is owned by the local client.
     *
     * An object is locally owned if it has ClientAuthority and its owner ID
     * matches the local client ID. Locally owned objects are not removed
     * during stale object cleanup.
     *
     * @param identity The NetworkIdentity to check.
     * @param gameWorld The game world context (provides local client ID).
     * @return true if the object is owned by the local client.
     */
    [[nodiscard]] bool isLocallyOwned(NetworkIdentity* identity, const GameWorld& gameWorld) const;

    /**
     * @brief Removes all networked objects from a scene.
     *
     * Called during scene transitions to properly clean up networked objects.
     * For each networked object:
     * - Calls onNetworkDespawn() on its NetworkIdentity
     * - Unregisters from the NetworkIdentityRegistry
     * - Removes tracking from NetworkSpawnManager
     * - Removes the GameObject from the scene
     *
     * @param scene The scene to clean up.
     */
    void cleanupNetworkObjectsInScene(Scene& scene);

private:
    /**
     * @brief Server-side scene processing.
     *
     * Extracts networked objects, registers them as prefabs, and spawns
     * ServerAuthority objects immediately.
     */
    void processForServer(Scene& scene, GameWorld& gameWorld);

    /**
     * @brief Client-side scene processing.
     *
     * Extracts networked objects and registers them as prefabs for later
     * instantiation when snapshots arrive.
     */
    void processForClient(Scene& scene);

    /**
     * @brief Updates an existing object with state from a snapshot.
     */
    void updateExistingObject(
        GameObject* existing,
        GameObject& received,
        GameWorld& gameWorld,
        std::vector<std::pair<GameObject*, uint32_t>>& toFixup,
        Scene& activeScene
    );

    /**
     * @brief Spawns a new object from a snapshot.
     */
    void spawnNewObject(
        std::unique_ptr<GameObject> received,
        GameWorld& gameWorld,
        std::vector<std::pair<GameObject*, uint32_t>>& toFixup,
        Scene& activeScene
    );

    /**
     * @brief Removes objects not present in the latest snapshot.
     */
    void removeStaleObjects(
        const std::unordered_set<uint32_t>& receivedNetIds,
        GameWorld& gameWorld,
        Scene& activeScene
    );

    /**
     * @brief Reloads sprite assets for a GameObject.
     */
    void reloadAssetsForGameObject(GameObject* obj, AssetManager& assetManager);

    /**
     * @brief Recursively adds inline children to the scene.
     */
    void addInlineChildrenRecursive(GameObject* obj, Scene& activeScene);

    /**
     * @brief Checks if a GameObject has any NetworkBehaviour components.
     */
    [[nodiscard]] bool hasNetworkBehaviour(const GameObject& obj) const;

    /**
     * @brief Checks if a GameObject has a NetworkIdentity component.
     */
    [[nodiscard]] bool hasNetworkIdentity(const GameObject& obj) const;

    /// @brief Pointer to the network spawn manager
    NetworkSpawnManager* spawnManager = nullptr;

    /// @brief Pointer to the behaviour system for initializing behaviours
    BehaviourSystem* behaviourSystem = nullptr;

    /// @brief Whether the system has been configured
    bool configured = false;

    /// @brief Frame counter for throttled logging
    int frameCount = 0;
};