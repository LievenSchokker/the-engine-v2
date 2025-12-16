#pragma once

#include "BaseMessageHandler.h"
#include "Networking/Messages/ConcreteMessages/ObjectDestroyMessage.h"

class NetworkSpawnManager;

class ObjectDestroyMessageHandler : public BaseMessageHandler<ObjectDestroyMessage>
{
public:
    ObjectDestroyMessageHandler(GameWorld& world, NetworkSpawnManager& spawnManager);
    ~ObjectDestroyMessageHandler() override = default;

protected:
    void handleMessageInternal() override;

private:
    NetworkSpawnManager& spawnManager;
};