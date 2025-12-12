#pragma once


#include "Behaviour/Behaviour.h"
#include "Networking/Messages/ConcreteMessages/ActionMessage.h"
#include "Networking/Serialization/Serialization.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Component/NetworkIdentity.h"

#include <functional>
#include <unordered_map>
#include <string>

#include "AuthorityType.h"

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
class NetworkBehaviour: public Behaviour
{
	using ActionCallback = std::function<void(ReadArchive&)>;

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
	 */;
	bool isClient() const;

	/**
	 * @brief Returns true if executing on a client.
	 */
	bool isServer() const;
	/**
	 * @brief Returns true if local machine has authority over this object.
	 *
	 * On server: always true for server-owned objects
	 * On client: true only if this client owns the object
	 */
	bool hasAuthority() const;

	GameWorld* getWorld();

	/**
	 * @brief Override to register Commands and RPCs using the NetworkBuilder.
	 *
	 * Called once during network spawn. Use the builder's fluent API:
	 *
	 *   builder.command("Move", [this](ReadArchive& ar) { ... });
	 *   builder.clientRpc("TakeDamage", [this](ReadArchive& ar) { ... });
	 */
	virtual void registerNetworkMethods(NetworkBuilder& builder)
	{
	}

	/**
	 * @brief Called when this object is spawned on the network.
	 *
	 * At this point, identity is valid and network methods are registered.
	 * Override to perform network-specific initialization.
	 */
	virtual void onNetworkSpawn()
	{
	}

	/**
	 * @brief Called when this object is removed from the network.
	 */
	virtual void onNetworkDespawn()
	{
	}

	/**
	 * @brief Called on clients when connection status changes.
	 */
	virtual void onClientConnectionStatusChanged(ConnectionStatus status)
	{
	}

	/**
	 * @brief Called on server when a client's connection status changes.
	 */
	virtual void onServerConnectionStatusChanged(
		int clientId, ConnectionStatus status)
	{
	}

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
	void executeAction(const std::string& actionKey);

	void setWorldRefrence(GameWorld* world);

	void setComponentNetworkId(uint32_t id);

	void addCommand(const std::string& name, ActionCallback callback);

    AuthorityType getAuthorityType() const;
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
	template <typename... Args>
	void callCommand(const std::string& name, Args&&... args);

	/**
	 * @brief Override to serialize state for network sync.
	 *
	 * Called by the networking layer when syncing state to clients.
	 * Write all variables that should be synchronized.
	 */
	virtual void writeStream(WriteArchive& archive)
	{
	}

	/**
	 * @brief Override to deserialize state from network sync.
	 *
	 * Called by the networking layer when receiving state updates.
	 * Read variables in the same order as writeStream().
	 */
	virtual void readStream(ReadArchive& archive)
	{
	}

	uint32_t componentNetworkId = 0;

    AuthorityType authorityType = AuthorityType::ServerAuthority;
private:
	GameWorld* world = nullptr;
	/// Commands are client→server calls
	std::unordered_map<std::string, ActionCallback> commands;
};

#include "NetworkBehaviour.hpp"