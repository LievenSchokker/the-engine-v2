#include "Networking/MessageHandlers/ActionMessageHandler.h"

#include "../../../inc/Component/NetworkIdentity.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkingIdentityRegistry.h"

#include <iostream>

ActionMessageHandler::ActionMessageHandler(GameWorld& world,
										   NetworkIdentityRegistry& registry)
	: BaseMessageHandler<ActionMessage>(world), identityRegistry(registry)
{
}

void ActionMessageHandler::handleMessageInternal()
{
	const ActionMessage* message = getMessage();

	if ( !message->validate() )
	{
		std::cerr << "[ActionMessageHandler] Invalid message\n";
		return;
	}

	const uint32_t netId = message->getGameObjectIdentity();

	const NetworkIdentity* identity = identityRegistry.findByNetId(netId);
	if ( !identity )
	{
		std::cerr << "[ActionMessageHandler] No object with netId=" << netId
				  << "\n";
		return;
	}

	identity->dispatchAction(message->getComponentIdentity(),
							 message->getAction(),
							 message->getPayload());
}