#pragma once


#include "Core/IEngineSystems.h"
#include "Networking/Messages/ConcreteMessages/StateSyncMessage.h"
#include "Networking/NetworkingIdentityRegistry.h"

class Server;

/**
 * @brief Collects and broadcasts object state at regular intervals.
 *
 * Called from the server's tick loop. Gathers transform data from all
 * networked objects and sends a batched StateSyncMessage to all clients.
 */
class StateSyncSystem
{
public:
	StateSyncSystem(Server* server, NetworkIdentityRegistry* registry);

	/**
	 * @brief Call every server tick. Broadcasts state every syncInterval ticks.
	 */
	void tick(uint32_t currentTick);

	/**
	 * @brief How often to sync (1 = every tick, 2 = every other tick, etc.)
	 */
	void setSyncInterval(uint32_t interval) { syncInterval = interval; }

private:
	void broadcastState(uint32_t currentTick);

	Server* server;
	NetworkIdentityRegistry* registry;
	uint32_t syncInterval = 1;  // Sync every tick by default
	uint32_t tickCounter = 0;
};