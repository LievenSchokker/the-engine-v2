    #pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <functional>

class NetworkIdentity;
class Scene;

/**
 * @brief Central registry for all networked objects in the current scene.
 *
 * Maintains fast lookup tables for routing incoming network messages to their
 * target GameObjects. Corresponds to your diagram's NetworkIdentityRegistry class.
 *
 *
 * Lifecycle:
 * - Objects register on network spawn via NetworkSpawnManager
 * - Objects unregister on network despawn or destruction
 * - Registry clears when scene unloads or client disconnects
 */
class NetworkIdentityRegistry
{
public:
    NetworkIdentityRegistry();
    ~NetworkIdentityRegistry();

    /**
     * @brief Associates the registry with the active scene.
     *
     * Called when a networked scene loads. The registry needs scene access
     * to find GameObjects during spawn operations.
     */
    void setCurrentScene(Scene* scene);

    /**
     * @brief Gets the currently associated scene.
     */
    Scene* getCurrentScene() const { return currentScene; }

    /**
     * @brief Registers a NetworkIdentity for network message routing.
     *
     * Called by NetworkSpawnManager after assigning a netId.
     * Fails silently if netId is already registered (prevents duplicates).
     *
     * @param identity The NetworkIdentity to register
     */
    void registerIdentity(NetworkIdentity* identity);

    /**
     * @brief Removes a NetworkIdentity from the registry.
     *
     * Called when an object is network-despawned or destroyed.
     * Safe to call with unregistered identity (no-op).
     *
     * @param identity The NetworkIdentity to unregister
     */
    void unregisterIdentity(NetworkIdentity* identity);

    /**
     * @brief Looks up a NetworkIdentity by its network ID.
     *
     * Primary lookup method for message routing. Returns nullptr if
     * not found, allowing callers to handle missing objects gracefully
     * (e.g., object destroyed before message arrived).
     *
     * @param netId The network identifier to search for
     * @return Pointer to the identity, or nullptr if not found
     */
    NetworkIdentity* findByNetId(uint32_t netId) const;

    /**
     * @brief Gets all registered NetworkIdentities.
     *
     * Used for broadcasting state updates and snapshot generation.
     * Returns a copy to prevent iterator invalidation if registry
     * is modified during iteration.
     */
    std::vector<NetworkIdentity*> getAllIdentities() const;

    /**
     * @brief Gets identities owned by a specific client.
     *
     * Useful for cleanup when a client disconnects - all their
     * owned objects may need special handling.
     *
     * @param ownerId Client ID, or -1 for server-owned objects
     */
    std::vector<NetworkIdentity*> getIdentitiesByOwner(int ownerId) const;

    /**
     * @brief Returns the number of registered identities.
     */
    size_t getCount() const { return identitiesByNetId.size(); }

    /**
     * @brief Removes all registered identities.
     *
     * Called on scene unload or disconnect. Does NOT despawn the objects,
     * only clears the registry's references.
     */
    void clear();

    /**
     * @brief Iterates over all identities with a callback.
     *
     * Safer than getAllIdentities() when you don't need to store results,
     * as it doesn't allocate a temporary vector.
     *
     * @param callback Function called for each identity
     */
    void forEach(const std::function<void(NetworkIdentity*)>& callback) const;

private:
    /// Primary lookup table: netId -> NetworkIdentity
    std::unordered_map<uint32_t, NetworkIdentity*> identitiesByNetId;

    /// Secondary index for owner-based queries
    std::unordered_map<int, std::vector<NetworkIdentity*>> identitiesByOwner;

    /// Reference to active scene for GameObject access
    Scene* currentScene = nullptr;

    /// Helper to update owner index when identity is added/removed
    void addToOwnerIndex(NetworkIdentity* identity);
    void removeFromOwnerIndex(NetworkIdentity* identity);
};