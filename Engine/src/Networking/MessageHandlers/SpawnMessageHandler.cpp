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
    std::cout << "Local client id: " << gameWorld->localClientId << std::endl;
	SpawnMessage* spawnMessage = getMessage();
    spawnManager.handleSpawnMessage(*spawnMessage);
}