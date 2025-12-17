#pragma once

#include "BaseMessageHandler.h"
#include "Networking/Messages/ConcreteMessages/ActionMessage.h"

class NetworkIdentityRegistry;

class ActionMessageHandler final: public BaseMessageHandler<ActionMessage>
{
public:
    ActionMessageHandler(GameWorld& world, NetworkIdentityRegistry& registry);
    ~ActionMessageHandler() override = default;

protected:
    void handleMessageInternal() override;

private:
    NetworkIdentityRegistry& identityRegistry;
};