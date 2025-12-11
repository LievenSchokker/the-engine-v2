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

		Transform* transform = go->getTransform();
		if (!transform) continue;

		Vector2 pos = transform->getPosition();
		float rot = static_cast<float>(transform->getRotationAngle());

		message.addObject(
			identity->getNetId(),
			pos.x,
			pos.y,
			rot
		);
	}



	if (!message.objects.empty())
	{
		server->broadcastMessage(message);
	}
}