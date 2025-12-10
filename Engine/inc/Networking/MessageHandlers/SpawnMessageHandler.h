#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Messages/ConcreteMessages/SpawnMessage.h"

class NetworkSpawnManager;

class SpawnMessageHandler : public BaseMessageHandler<SpawnMessage>
{
public:
    SpawnMessageHandler(GameWorld& world, NetworkSpawnManager& spawnManager);
    ~SpawnMessageHandler() override = default;

protected:
    void handleMessageInternal() override;

private:
	GameWorld& world;
	NetworkSpawnManager& spawnManager;
};