#pragma once

#include "BaseMessageHandler.h"
#include "Networking/Messages/ConcreteMessages/StateSyncMessage.h"

class NetworkIdentityRegistry;

class StateSyncMessageHandler : public BaseMessageHandler<StateSyncMessage>
{
public:
    StateSyncMessageHandler(GameWorld& world, NetworkIdentityRegistry& registry);
    ~StateSyncMessageHandler() override = default;

protected:
    void handleMessageInternal() override;

private:
    NetworkIdentityRegistry& registry;
};