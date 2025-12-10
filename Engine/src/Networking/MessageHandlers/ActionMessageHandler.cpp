#include "Networking/MessageHandlers/ActionMessageHandler.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "Networking/NetworkIdentity.h"
#include "GameObject/GameObject.h"
#include <iostream>

ActionMessageHandler::ActionMessageHandler(GameWorld& world, NetworkIdentityRegistry& registry)
    : BaseMessageHandler<ActionMessage>(world)
    , identityRegistry(registry)
{
}

void ActionMessageHandler::handleMessageInternal()
{
    const ActionMessage* message = getMessage();

    if (!message->validate())
    {
        std::cerr << "[ActionMessageHandler] Invalid message\n";
        return;
    }

    uint32_t netId = message->getGameObjectIdentity();

    NetworkIdentity* identity = identityRegistry.findByNetId(netId);
    if (!identity)
    {
        std::cerr << "[ActionMessageHandler] No object with netId=" << netId << "\n";
        return;
    }

    std::cout << "Handling action message from netId=" << netId << std::endl;
    identity->dispatchAction(
        message->getComponentIdentity(),
        message->getAction()
    );
}