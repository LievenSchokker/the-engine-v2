#include "Networking/MessageHandlers/StateSyncHandler.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "../../../inc/Component/NetworkIdentity.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include <iostream>

StateSyncMessageHandler::StateSyncMessageHandler(GameWorld& world, NetworkIdentityRegistry& registry)
    : BaseMessageHandler<StateSyncMessage>(world)
    , registry(registry)
{
}

void StateSyncMessageHandler::handleMessageInternal()
{
    const StateSyncMessage* syncMsg = getMessage();

    for (const auto& objState : syncMsg->objects)
    {

        const NetworkIdentity* identity = registry.findByNetId(objState.netId);
        if (!identity)
        {
            continue;
        }

        const GameObject* go = identity->getGameObject();
        if (!go) continue;

        Transform* transform = go->getTransform();
        if (!transform) continue;

        transform->setPosition({objState.posX, objState.posY});
        transform->setRotationAngle(objState.rotation);
    }
}