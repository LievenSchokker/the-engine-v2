#include "Networking/Messages/Concretes/StateSyncMessage.h"
#include "Networking/NetworkIdentity.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Networking/MessageHandlers/StateSyncHandler.h"

StateSyncMessageHandler::StateSyncMessageHandler(
	NetworkContext& context,
	NetworkIdentityRegistry& registry)
	: context(context)
	, registry(registry)
{
}

void StateSyncMessageHandler::handle(const IMessage& message)
{
	const auto& syncMsg = static_cast<const StateSyncMessage&>(message);

	std::cout << "[Client] Received StateSync with "
			  << syncMsg.objects.size() << " objects" << std::endl;

	for (const auto& objState : syncMsg.objects)
	{
		std::cout << "[Client] Processing netId=" << objState.netId
				  << " pos=(" << objState.posX << ", " << objState.posY << ")" << std::endl;

		NetworkIdentity* identity = registry.findByNetId(objState.netId);
		if (!identity)
		{
			std::cout << "[Client] Identity not found for netId=" << objState.netId << std::endl;
			continue;
		}


		GameObject* go = identity->getGameObject();
		if (!go) continue;

		Transform* transform = go->getTransform();
		if (!transform) continue;

		transform->setPosition({objState.posX, objState.posY});
		transform->setRotationAngle(objState.rotation);

		std::cout << "[Client] Applied position!" << std::endl;
	}
}