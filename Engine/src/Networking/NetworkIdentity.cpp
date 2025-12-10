#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkBuilder.h"

bool NetworkIdentity::hasAuthority()
{
	if (!gameWorld) return false;

	if (gameWorld->isServer())
	{
		return true;
	}
	return ownerId == gameWorld->localClientId;
}

void NetworkIdentity::onNetworkSpawn()
{
	networkBehaviours.clear();

	const auto& allBehaviours = getGameObject()->getAllBehaviours();
	for (Behaviour* behaviour : allBehaviours)
	{
		if (auto* netBehaviour = dynamic_cast<NetworkBehaviour*>(behaviour))
		{
			netBehaviour->world = gameWorld;
			netBehaviour->componentNetworkId = static_cast<uint32_t>(
				networkBehaviours.size());
			netBehaviour->identity = this;
			networkBehaviours.push_back(netBehaviour);

			NetworkBuilder builder(*netBehaviour);
			netBehaviour->registerNetworkMethods(builder);

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

void NetworkIdentity::serialize(CerealWriteArchive& archive) const
{
}

void NetworkIdentity::deserialize(CerealReadArchive& archive)
{
}

void NetworkIdentity::dispatchAction(uint32_t componentId,
                                     const std::string& action) const
{
	if (componentId >= networkBehaviours.size())
	{
		return;
	}

	if (NetworkBehaviour* behaviour = networkBehaviours[componentId])
	{
		behaviour->executeAction(action);
	}
}

GameWorld* NetworkIdentity::getWorld()
{
	return gameWorld;
}