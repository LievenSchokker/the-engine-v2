#include "Networking/MessageHandlers/ActionMessageHandler.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "Networking/NetworkIdentity.h"
#include "Networking/Messages/Concretes/ActionMessage.h"

ActionMessageHandler::ActionMessageHandler(NetworkContext& context,
                                           NetworkIdentityRegistry& registry)
    : BaseMessageHandler(context)
    , identityRegistry(registry)
{
}

void ActionMessageHandler::handleMessageInternal(const ActionMessage& message)
{
    // Validate message
    if (!message.validate())
    {
        return;
    }

    // Find the target NetworkIdentity
    NetworkIdentity* identity = identityRegistry.findByNetId(message.getGameObjectIdentity());
    if (!identity)
    {
        // Object doesn't exist locally (may have been destroyed)
        return;
    }


    identity->dispatchAction(
        message.getComponentIdentity(),
        message.getAction()
    );
}