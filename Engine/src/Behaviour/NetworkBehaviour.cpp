#include "Behaviour/NetworkBehaviour.h"

#include "Core/GameWorld.h"

NetworkBehaviour::NetworkBehaviour() = default;

bool NetworkBehaviour::isServer() const
{
    return gameWorld && gameWorld->isServer();
}

bool NetworkBehaviour::isClient() const
{
    return gameWorld && gameWorld->isClient();
}

bool NetworkBehaviour::hasAuthority() const
{
    if (!identity) return false;
    return identity->hasAuthority();
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

void NetworkBehaviour::setComponentNetworkId(uint32_t id)
{
	componentNetworkId = id;
}
