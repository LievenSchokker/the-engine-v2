#pragma once

#include "IMessageHandler.h"

class NetworkSpawnManager;

class SpawnMessageHandler : public IMessageHandler
{
public:
    explicit SpawnMessageHandler(NetworkSpawnManager& spawnManager);
    void handle(const IMessage& message) override;

private:
    NetworkSpawnManager& spawnManager;
};