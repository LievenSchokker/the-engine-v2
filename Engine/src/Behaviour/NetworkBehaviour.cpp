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

void NetworkBehaviour::executeAction(const std::string& actionKey, const std::byte* payload, size_t payloadLength)
{
    ReadArchive archive(payload, payloadLength);

    // Check commands first (client→server)
    auto cmdIt = commands.find(actionKey);
    if (cmdIt != commands.end())
    {
        cmdIt->second(archive);
        return;
    }

    // Then check RPCs (server→client)
    auto rpcIt = rpcs.find(actionKey);
    if (rpcIt != rpcs.end())
    {
        rpcIt->second(archive);
    }
}