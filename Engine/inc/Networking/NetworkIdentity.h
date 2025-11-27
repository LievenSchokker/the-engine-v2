#pragma once

#include "Component/Component.h"
#include <cstdint>
#include <vector>

class NetworkBehaviour;

/**
 * @brief Identifies a GameObject on the network and manages its NetworkBehaviours.
 *
 * Every networked GameObject must have exactly one NetworkIdentity component.
 * It is automatically added when using NetworkSpawnManager and provides:
 * - Unique network ID for message routing
 * - Ownership tracking for authority checks
 * - Registry of NetworkBehaviours for action dispatch
 *
 * This corresponds to your diagram's NetworkIdentity class with netId and ownerId.
 */
class NetworkIdentity : public Component
{
public:
    NetworkIdentity() = default;
    ~NetworkIdentity() override = default;

    /**
     * @brief Unique network identifier assigned at spawn.
     *
     * Maps to ActionMessage::networkGameObjectIdentity for routing.
     */
    uint32_t getNetId() const { return netId; }

    /**
     * @brief Client ID of the owner, or -1 for server-owned objects.
     *
     * Used for authority checks in NetworkBehaviour.
     */
    int getOwnerId() const { return ownerId; }

    /**
     * @brief Checks if the local machine has authority.
     *
     * Server always has authority over server-owned objects.
     * Clients only have authority over objects they own.
     */
    bool hasAuthority() const;

    /**
     * @brief Called by NetworkSpawnManager when spawned on network.
     *
     * Collects all NetworkBehaviours, assigns component IDs,
     * and calls their onNetworkSpawn() methods.
     */
    void onNetworkSpawn();

    /**
     * @brief Called by NetworkSpawnManager when despawned.
     */
    void onNetworkDespawn();

    /**
     * @brief Routes an ActionMessage to the appropriate NetworkBehaviour.
     *
     * @param componentId The component index from ActionMessage
     * @param action The action key
     * @param payload
     * @param payloadSize
     * @param payload Serialized arguments
     * @param length Payload size
     */
    void dispatchAction(uint32_t componentId, const std::string& action, const std::byte* payload, size_t payloadSize);

private:
    uint32_t netId = 0;
    int ownerId = -1;

    /// Indexed by componentNetworkId for O(1) dispatch
    std::vector<NetworkBehaviour*> networkBehaviours;

    friend class NetworkSpawnManager;
    friend class SpawnMessageHandler;
};