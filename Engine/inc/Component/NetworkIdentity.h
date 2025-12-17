#pragma once

#include "BaseComponentTypes/Component.h"
#include <cstdint>
#include <vector>

#include "BaseComponentTypes/ComponentBase.h"

class NetworkBehaviour;
class GameWorld;
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
class NetworkIdentity final : public ComponentBase<NetworkIdentity>
{
public:
    NetworkIdentity() = default;
    ~NetworkIdentity() override;

	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;
    static constexpr const char* name() { return "NetworkIdentity"; }
    /**
     * @brief Unique network identifier assigned at spawn.
     *
     * Maps to ActionMessage::networkGameObjectIdentity for routing.
     */
    uint32_t getNetId() const { return networkId; }

    /**
     * @brief Client ID of the owner, or -1 for server-owned objects.
     *
     * Used for authority checks in NetworkBehaviour.
     */
    int getOwnerId() const;

    /**
     * @brief Checks if the local machine has authority.
     *
     * Server always has authority over server-owned objects.
     * Clients only have authority over objects they own.
     *
     * Authority here does not mean they own the state.
     * It means they have authority to perform an action on this Behaviour
     */
    bool hasAuthority() const;
    void onNetworkInstantiate(uint32_t nextNetworkId);

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
    void dispatchAction(uint32_t componentId, const std::string& action) const;

    GameWorld* getWorld() const;
    void setWorld(GameWorld* world);
private:
    GameWorld* gameWorld{};
    uint32_t networkId = 0;
    int ownerId = -1;

    /// Indexed by componentNetworkId for O(1) dispatch
    std::vector<NetworkBehaviour*> networkBehaviours;

    friend class NetworkSpawnManager;
    friend class SpawnMessageHandler;
};