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

inline void NetworkBehaviour::addCommand(const std::string& name, ActionCallback callback)
{
	commands[name] = std::move(callback);
}