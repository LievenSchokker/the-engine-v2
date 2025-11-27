#include "Networking/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "GameObject/GameObject.h"

bool NetworkIdentity::hasAuthority() const
{
    auto* world = getWorld();
    if (!world) return false;

    if (world->isServer())
    {
        return true;
    }
    return ownerId == world->localClientId;
}

void NetworkIdentity::onNetworkSpawn()
{
    networkBehaviours.clear();

    const auto& allBehaviours = getGameObject()->getAllBehaviours();
    for (Behaviour* behaviour : allBehaviours)
    {
        if (auto* netBehaviour = dynamic_cast<NetworkBehaviour*>(behaviour))
        {
            netBehaviour->componentNetworkId = static_cast<uint32_t>(networkBehaviours.size());
            netBehaviour->identity = this;
            networkBehaviours.push_back(netBehaviour);
            netBehaviour->onNetworkSpawn();
        }
    }
}

void NetworkIdentity::onNetworkDespawn()
{
    for (auto* behaviour : networkBehaviours)
    {
        if (behaviour)
        {
            behaviour->onNetworkDespawn();
        }
    }
    networkBehaviours.clear();
}

void NetworkIdentity::dispatchAction(uint32_t componentId, const std::string& action,
                                      const std::byte* payload, size_t payloadSize)
{
    if (componentId >= networkBehaviours.size())
    {
        return;
    }

    NetworkBehaviour* behaviour = networkBehaviours[componentId];
    if (behaviour)
    {
        behaviour->executeAction(action, payload, payloadSize);
    }
}





