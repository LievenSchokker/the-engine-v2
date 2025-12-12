#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkBuilder.h"
#include "Networking/NetworkSpawnManager.h"

NetworkIdentity::~NetworkIdentity()
{
	if (gameWorld && gameWorld->spawnManager)
	{
		gameWorld->spawnManager->getNetworkIdentityRegistry().unregisterIdentity(this);
	}
}


bool NetworkIdentity::hasAuthority() const
{
	if (!gameWorld) return false;

	if (gameWorld->isServer())
	{
		return true;
	}
	return ownerId == gameWorld->localClientId;
}

void NetworkIdentity::onNetworkInstantiate()
{
	if (getWorld()->isServer() == false) return;

	networkBehaviours.clear();

	const auto& allBehaviours = getGameObject()->getAllBehaviours();
	uint32_t componentId = 0;

	for (Behaviour* behaviour : allBehaviours)
	{
		if (auto* netBehaviour = dynamic_cast<NetworkBehaviour*>(behaviour))
		{
			netBehaviour->setComponentNetworkId(componentId++);
			networkBehaviours.push_back(netBehaviour);
		}
	}
}

void NetworkIdentity::onNetworkSpawn()
{
	for (const auto& allBehaviours = getGameObject()->getAllBehaviours(); Behaviour* behaviour : allBehaviours)
	{
		if (auto* netBehaviour = dynamic_cast<NetworkBehaviour*>(behaviour))
		{
			netBehaviour->setWorldRefrence(gameWorld);
			netBehaviour->identity = this;

			if (std::ranges::find(networkBehaviours, netBehaviour) == networkBehaviours.end())
			{
				networkBehaviours.push_back(netBehaviour);
			}

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

int NetworkIdentity::getOwnerId() const
{
	return ownerId;
}


void NetworkIdentity::dispatchAction(const uint32_t componentId,
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

GameWorld* NetworkIdentity::getWorld() const
{
	return gameWorld;
}