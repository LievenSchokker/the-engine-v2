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
    auto cmdIt = commands.find(actionKey);
}