#pragma once

#include "BaseMessageHandler.h"
#include "Networking/MessageHandlers/BaseMessageHandler.h"
#include "Networking/Messages/Concretes/ActionMessage.h"

class NetworkIdentityRegistry;

/**
 * @brief Handles incoming ActionMessages by routing to NetworkBehaviours.
 *
 * Follows your existing BaseMessageHandler pattern. When an ActionMessage
 * arrives, this handler:
 * 1. Looks up the target GameObject by networkGameObjectIdentity
 * 2. Finds the NetworkBehaviour by networkComponentIdentity
 * 3. Calls executeAction() with the action key and payload
 */
class ActionMessageHandler : public BaseMessageHandler<ActionMessage>
{
public:
    ActionMessageHandler(NetworkContext& context, NetworkIdentityRegistry& registry);
    ~ActionMessageHandler() override = default;

protected:
    void handleMessageInternal(const ActionMessage& message) override;

private:
    NetworkIdentityRegistry& identityRegistry;
};