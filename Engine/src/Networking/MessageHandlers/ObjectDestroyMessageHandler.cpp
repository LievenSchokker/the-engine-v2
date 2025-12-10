#include "Networking/MessageHandlers/ObjectDestroyMessageHandler.h"
#include "Networking/NetworkSpawnManager.h"
#include "Behaviour/NetworkBehaviour.h"

ObjectDestroyMessageHandler::ObjectDestroyMessageHandler(GameWorld& world, NetworkSpawnManager& spawnManager)
    : BaseMessageHandler<ObjectDestroyMessage>(world)
    , spawnManager(spawnManager)
{
}

void ObjectDestroyMessageHandler::handleMessageInternal()
{
    const ObjectDestroyMessage* destroyMsg = getMessage();
}