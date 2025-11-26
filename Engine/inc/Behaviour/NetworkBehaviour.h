#pragma once

#include "Behaviour/Behaviour.h"
#include "Networking/Messages/Concretes/ActionMessage.h"
#include "Networking/Serialization/Serialization.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/NetworkIdentity.h"
#include <functional>
#include <unordered_map>
#include <string>

class NetworkBuilder;

/**
 * @brief Base class for components that participate in networked gameplay.
 *
 * NetworkBehaviour extends Behaviour with networking capabilities, allowing
 * derived classes to register Commands (client→server) and RPCs (server→clients).
 *
 * This design mirrors the ActionMessage pattern already in the codebase:
 * NetworkBehaviours emit ActionMessages that flow through the existing
 * MessageDispatcher infrastructure.
 *
 * Usage pattern:
 * 1. Derive from NetworkBehaviour
 * 2. Override registerNetworkMethods() to register commands/RPCs
 * 3. Call callCommand()/callRpc() to invoke remote methods
 */
class NetworkBehaviour : public Behaviour
{
public:
    NetworkBehaviour();
    ~NetworkBehaviour() override = default;

    /**
     * @brief Reference to the NetworkIdentity on the same GameObject.
     *
     * Set automatically when the object is network-spawned. Provides
     * access to netId for message routing and authority checks.
     */
    NetworkIdentity* identity = nullptr;

    /**
     * @brief Returns true if executing on the server.
     *
     * Uses NetworkContext/ProgramType to determine runtime mode.
     */
    bool isServer() const;

    /**
     * @brief Returns true if executing on a client.
     */
    bool isClient() const;

    /**
     * @brief Returns true if local machine has authority over this object.
     *
     * On server: always true for server-owned objects
     * On client: true only if this client owns the object
     */
    bool hasAuthority() const;

    /**
     * @brief Override to register Commands and RPCs using the NetworkBuilder.
     *
     * Called once during network spawn. Use the builder's fluent API:
     *
     *   builder.command("Move", [this](ReadArchive& ar) { ... });
     *   builder.clientRpc("TakeDamage", [this](ReadArchive& ar) { ... });
     */
    virtual void registerNetworkMethods(NetworkBuilder& builder) {}

    /**
     * @brief Called when this object is spawned on the network.
     *
     * At this point, identity is valid and network methods are registered.
     * Override to perform network-specific initialization.
     */
    virtual void onNetworkSpawn() {}

    /**
     * @brief Called when this object is removed from the network.
     */
    virtual void onNetworkDespawn() {}

    /**
     * @brief Called on clients when connection status changes.
     */
    virtual void onClientConnectionStatusChanged(ConnectionStatus status) {}

    /**
     * @brief Called on server when a client's connection status changes.
     */
    virtual void onServerConnectionStatusChanged(int clientId, ConnectionStatus status) {}

    /**
     * @brief Invokes a registered command/RPC by name with the given payload.
     *
     * Called by the ActionMessageHandler when an ActionMessage arrives
     * targeting this component. Routes to the appropriate registered callback.
     *
     * @param actionKey The command/RPC name from ActionMessage::getAction()
     * @param payload Serialized arguments from ActionMessage
     * @param payloadLength Size of payload in bytes
     */
    void executeAction(const std::string& actionKey, const std::byte* payload, size_t payloadLength);

protected:
    /**
     * @brief Sends a command from client to server.
     *
     * Commands are requests from the owning client to the server.
     * Only valid to call on clients with authority over this object.
     *
     * @tparam Args Types of arguments to serialize
     * @param name The registered command name
     * @param args Arguments to pass to the server-side handler
     */
    template<typename... Args>
    void callCommand(const std::string& name, Args&&... args);

    /**
     * @brief Sends an RPC from server to all clients.
     *
     * RPCs are broadcasts from server to all connected clients.
     * Only valid to call on the server.
     *
     * @tparam Args Types of arguments to serialize
     * @param name The registered RPC name
     * @param args Arguments to pass to client-side handlers
     */
    template<typename... Args>
    void callRpc(const std::string& name, Args&&... args);

    /**
     * @brief Sends an RPC from server to a specific client.
     *
     * @param name The registered RPC name
     * @param targetClientId The client to send to
     * @param args Arguments to pass to the client-side handler
     */
    template<typename... Args>
    void callTargetRpc(const std::string& name, int targetClientId, Args&&... args);

    /**
     * @brief Override to serialize state for network sync.
     *
     * Called by the networking layer when syncing state to clients.
     * Write all variables that should be synchronized.
     */
    virtual void writeStream(WriteArchive& archive) {}

    /**
     * @brief Override to deserialize state from network sync.
     *
     * Called by the networking layer when receiving state updates.
     * Read variables in the same order as writeStream().
     */
    virtual void readStream(ReadArchive& archive) {}
    uint32_t componentNetworkId = 0;

private:
    using ActionCallback = std::function<void(ReadArchive&)>;

    /// Commands are client→server calls
    std::unordered_map<std::string, ActionCallback> commands;

    /// RPCs are server→client calls
    std::unordered_map<std::string, ActionCallback> rpcs;

    /// Unique identifier for this component type on the GameObject
    /// Used in ActionMessage routing (networkComponentIdentity field)
    friend class NetworkBuilder;
    friend class ActionMessageHandler;
    friend class NetworkSpawnManager;
    friend class NetworkIdentity;
};

// ============================================================================
// Template Implementations
// ============================================================================

template<typename... Args>
void NetworkBehaviour::callCommand(const std::string& name, Args&&... args)
{
    if (!isClient() || !identity)
    {
        return;
    }

    // Serialize arguments
    WriteArchive archive;
    // Fold expression to serialize all args
    (archive.process(args), ...);

    // Create ActionMessage targeting this component on the server
    ActionMessage message(
        componentNetworkId,
        identity->getNetId(),
        name,
        10 // Assumes access to simulation tick
    );

    //sendToServer(message);
}

template<typename... Args>
void NetworkBehaviour::callRpc(const std::string& name, Args&&... args)
{
    if (!isServer() || !identity)
    {
        return;
    }

    WriteArchive archive;
    (archive.process(args), ...);

    ActionMessage message(
        componentNetworkId,
        identity->getNetId(),
        name,
        60
    );

    // Broadcast to all clients
    //broadcastToClients(message);
}

template<typename... Args>
void NetworkBehaviour::callTargetRpc(const std::string& name, int targetClientId, Args&&... args)
{
    if (!isServer() || !identity)
    {
        return;
    }

    WriteArchive archive;
    (archive.process(args), ...);

    ActionMessage message(
        componentNetworkId,
        identity->getNetId(),
        name,
        60
    );

    //sendToClient(targetClientId, message);
}