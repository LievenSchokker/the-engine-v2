#include "Networking/MessageHandlers/SpawnMessageHandler.h"
#include "Networking/NetworkSpawnManager.h"
#include <iostream>

SpawnMessageHandler::SpawnMessageHandler(GameWorld& world, NetworkSpawnManager& spawnManager)
    : BaseMessageHandler<SpawnMessage>(world)
    , spawnManager(spawnManager)
{
}

void SpawnMessageHandler::handleMessageInternal()
{
    std::cout << "Received spawn message" << std::endl;
    const SpawnMessage* spawnMsg = getMessage();
    spawnManager.handleSpawnMessage(*spawnMsg);
}