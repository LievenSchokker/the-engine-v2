#include "Networking/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "GameObject/GameObject.h"

bool NetworkIdentity::hasAuthority() const
{
    return ownerId == -1;
}

void NetworkIdentity::onNetworkSpawn()
{
    networkBehaviours.clear();

    // Get all Behaviours and filter for NetworkBehaviours
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

void NetworkIdentity::dispatchAction(uint32_t componentId, const std::string& action)
{
    if (componentId >= networkBehaviours.size())
    {
        return;
    }

    NetworkBehaviour* behaviour = networkBehaviours[componentId];
    if (behaviour)
    {
        // ActionMessage doesn't carry payload, so pass nullptr/0
        behaviour->executeAction(action, nullptr, 0);
    }
}