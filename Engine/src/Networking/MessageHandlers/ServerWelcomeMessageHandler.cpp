#include "Networking/MessageHandlers/ServerWelcomeMessageHandler.h"
#include "Networking/NetworkSpawnManager.h"
#include "Behaviour/NetworkBehaviour.h"
#include "Scene/SceneManager.h"

ServerWelcomeMessageHandler::ServerWelcomeMessageHandler(GameWorld& world, NetworkSpawnManager& spawnManager)
    : BaseMessageHandler<WelcomeMessage>(world)
    , spawnManager(spawnManager)
{
}

void ServerWelcomeMessageHandler::handleMessageInternal()
{
    const WelcomeMessage* message = getMessage();
    if (!message) return;
    spawnManager.CheckNewClientSpawnObject(message->getClientId());
}