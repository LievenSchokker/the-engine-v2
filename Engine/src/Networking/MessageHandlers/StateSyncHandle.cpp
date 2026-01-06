#include "Networking/MessageHandlers/StateSyncHandler.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "../../../inc/Component/NetworkIdentity.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Scene/SceneManager.h"

#include <iostream>

#include "Networking/Client.h"

StateSyncMessageHandler::StateSyncMessageHandler(GameWorld& world, NetworkIdentityRegistry& registry)
    : BaseMessageHandler<StateSyncMessage>(world)
    , registry(registry)
{
}

void StateSyncMessageHandler::handleMessageInternal()
{
	StateSyncMessage* message = getMessage();
	if (!message) return;


    uint32_t lastTick = gameWorld->client->getLastReceivedTick();
    // Skip check if we haven't received anything yet
    if (message->tick <= lastTick && message->tick != 0)
    {
        return;
    }

    gameWorld->client->setLastReceivedTick(message->tick);
	gameWorld->sceneManager->applyNetworkSnapshot(message->gameObjects);
}