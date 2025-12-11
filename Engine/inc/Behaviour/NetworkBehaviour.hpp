#include "NetworkBehaviour.h"
#include "Core/GameWorld.h"

template<typename... Args>
void NetworkBehaviour::callCommand(const std::string& name, Args&&... args)
{
	if (!isClient() || !identity)
	{
		return;
	}

	//auto* world = getWorld();
	if (!world)
	{
		return;
	}

	const ActionMessage message(
		componentNetworkId,
		identity->getNetId(),
		name,
		0
	);

	world->sendToServer(message);
}

template<typename... Args>
void NetworkBehaviour::callRpc(const std::string& name, Args&&... args)
{
	if (!isServer() || !identity)
	{
		return;
	}

	auto* world = getWorld();
	if (!world)
	{
		return;
	}

	const ActionMessage message(
		componentNetworkId,
		identity->getNetId(),
		name,
		0
	);

	world->broadcastToClients(message);
}

template<typename... Args>
void NetworkBehaviour::callTargetRpc(const std::string& name, int targetClientId, Args&&... args)
{
	if (!isServer() || !identity)
	{
		return;
	}

	auto* world = getWorld();
	if (!world)
	{
		return;
	}

	ActionMessage message(
		componentNetworkId,
		identity->getNetId(),
		name,
		0
	);

	world->sendToClient(targetClientId, message);
}