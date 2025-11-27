#pragma once
#include "IMessageHandler.h"


class ObjectDestroyMessageHandler : public IMessageHandler
{
public:
    ObjectDestroyMessageHandler(NetworkSpawnManager& spawnManager);
    void handleMessage(const IMessage& message) override;
private:
    NetworkSpawnManager& spawnManager;
};
