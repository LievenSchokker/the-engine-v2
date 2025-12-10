#include "Networking/MessageHandlers/SpawnMessageHandler.h"
#include "Networking/NetworkSpawnManager.h"
#include <iostream>

SpawnMessageHandler::SpawnMessageHandler(GameWorld& world, NetworkSpawnManager& spawnManager)
	: BaseMessageHandler<SpawnMessage>(world), world(world),
	  spawnManager(spawnManager)
{
}

void SpawnMessageHandler::handleMessageInternal()
{
    const SpawnMessage* spawnMsg = getMessage();
    spawnManager.handleSpawnMessage(*spawnMsg);
}