#include "Networking/MessageHandlers/ActionMessageHandler.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "Networking/NetworkIdentity.h"
#include "GameObject/GameObject.h"
#include <iostream>

ActionMessageHandler::ActionMessageHandler(NetworkContext& context, NetworkIdentityRegistry& registry)
    : BaseMessageHandler<ActionMessage>(context)
    , identityRegistry(registry)
{
}

void ActionMessageHandler::handleMessageInternal(const ActionMessage& message)
{
    if (!message.validate())
    {
        std::cerr << "[ActionMessageHandler] Invalid message\n";
        return;
    }

    uint32_t netId = message.getGameObjectIdentity();

    NetworkIdentity* identity = identityRegistry.findByNetId(netId);
    if (!identity)
    {
        std::cerr << "[ActionMessageHandler] No object with netId=" << netId << "\n";
        return;
    }

    identity->dispatchAction(
        message.getComponentIdentity(),
        message.getAction()
    );
}