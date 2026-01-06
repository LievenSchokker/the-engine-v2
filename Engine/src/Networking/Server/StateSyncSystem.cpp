#include "Networking/Server/StateSyncSystem.h"
#include "Networking/Server/Server.h"
#include "../../../inc/Component/NetworkIdentity.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"

StateSyncSystem::StateSyncSystem(Server* server, NetworkIdentityRegistry* registry)
	: server(server)
	, registry(registry)
{
}

void StateSyncSystem::tick(uint32_t currentTick)
{
	tickCounter++;

	if (tickCounter >= syncInterval)
	{
		tickCounter = 0;
		broadcastState(currentTick);
	}
}

void StateSyncSystem::broadcastState(uint32_t currentTick)
{
	StateSyncMessage message;
	message.tick = currentTick;
	for (auto* identity : registry->getAllIdentities())
	{
		if (!identity) continue;
		GameObject* go = identity->getGameObject();
		if (!go) continue;
		message.addGameObject(go->clone());
	}

	if (!message.gameObjects.empty())
	{
		server->broadcastMessage(message);
	}

}