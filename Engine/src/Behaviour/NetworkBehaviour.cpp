#include "Behaviour/NetworkBehaviour.h"

#include "Core/GameWorld.h"

NetworkBehaviour::NetworkBehaviour() = default;

bool NetworkBehaviour::isServer() const
{
    return world && world->isServer();
}

bool NetworkBehaviour::isClient() const
{
    return world && world->isClient();
}

bool NetworkBehaviour::hasAuthority() const
{
    if (!identity) return false;
    return identity->hasAuthority();
}

GameWorld* NetworkBehaviour::getWorld()
{
	return world;
}

void NetworkBehaviour::executeAction(const std::string& actionKey)
{
	auto commandEntry = commands.find(actionKey);

	if (commandEntry != commands.end())
	{
		ReadArchive emptyArchive(nullptr, 0);
		commandEntry->second(emptyArchive);
	}
}