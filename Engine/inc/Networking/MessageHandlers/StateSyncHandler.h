#pragma once

#include "IMessageHandler.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "Networking/Messages/IMessage.h"

class NetworkContext;

/**
 * @brief Handles incoming StateSyncMessage on clients.
 *
 * Applies received transform state to all non-owned networked objects.
 */
class StateSyncMessageHandler : public IMessageHandler
{
public:
	StateSyncMessageHandler(NetworkContext& context, NetworkIdentityRegistry& registry);
	void handle(const IMessage& message) override;

private:
	NetworkContext& context;
	NetworkIdentityRegistry& registry;
};