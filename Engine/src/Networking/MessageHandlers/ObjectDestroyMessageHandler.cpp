


#include "Behaviour/NetworkBehaviour.h"
#include "Networking/Messages/Concretes/ObjectDestroyMessage.h"
#include "Networking/MessageHandlers/ObjectDestroyMessageHandler.h"

ObjectDestroyMessageHandler::ObjectDestroyMessageHandler(NetworkSpawnManager& spawnManager)
    : spawnManager(spawnManager)
{
}

void ObjectDestroyMessageHandler::handle(const IMessage& message)
{
    const auto& destroyMsg = static_cast<const ObjectDestroyMessage&>(message);
    // Client-side: just remove from tracking and destroy
    // The spawnManager needs a client-side despawn method
}