#include "Networking/MessageHandlers/StateSyncHandler.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "../../../inc/Component/NetworkIdentity.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Scene/SceneManager.h"

#include <iostream>

StateSyncMessageHandler::StateSyncMessageHandler(GameWorld& world, NetworkIdentityRegistry& registry)
    : BaseMessageHandler<StateSyncMessage>(world)
    , registry(registry)
{
}

void StateSyncMessageHandler::handleMessageInternal()
{
	StateSyncMessage* message = getMessage();
	if (!message) return;

	gameWorld->sceneManager->applyNetworkSnapshot(message->gameObjects);
}