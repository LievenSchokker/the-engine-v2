#include "Networking/MessageHandlers/SpawnMessageHandler.h"

#include <iostream>

#include "Networking/NetworkSpawnManager.h"
#include "Networking/Messages/Concretes/SpawnMessage.h"

SpawnMessageHandler::SpawnMessageHandler(NetworkSpawnManager& spawnManager)
    : spawnManager(spawnManager)
{
}

void SpawnMessageHandler::handle(const IMessage& message)
{
    std::cout << "Received spawn message" << std::endl;
    const auto& spawnMsg = static_cast<const SpawnMessage&>(message);
    spawnManager.handleSpawnMessage(spawnMsg);
}