#include "Behaviour/NetworkBehaviour.h"

NetworkBehaviour::NetworkBehaviour() = default;

bool NetworkBehaviour::isServer() const
{
    auto* world = getWorld();
    return world && world->isServer();
}

bool NetworkBehaviour::isClient() const
{
    auto* world = getWorld();
    return world && world->isClient();
}

bool NetworkBehaviour::hasAuthority() const
{
    if (!identity) return false;
    return identity->hasAuthority();
}

void NetworkBehaviour::executeAction(const std::string& actionKey)
{
	// Check commands map (client → server)
	auto cmdIt = commands.find(actionKey);
	if (cmdIt != commands.end())
	{
		ReadArchive emptyArchive(nullptr, 0);
		cmdIt->second(emptyArchive);  // <-- Actually call the callback!
		return;
	}

	// Check RPCs map (server → client)
	auto rpcIt = rpcs.find(actionKey);
	if (rpcIt != rpcs.end())
	{
		ReadArchive emptyArchive(nullptr, 0);
		rpcIt->second(emptyArchive);
		return;
	}

	std::cerr << "[NetworkBehaviour] Unknown action: " << actionKey << std::endl;
}