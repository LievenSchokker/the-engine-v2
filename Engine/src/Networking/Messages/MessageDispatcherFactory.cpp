#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/MessageHandlers/SpawnMessageHandler.h"
#include "Networking/MessageHandlers/ObjectDestroyMessageHandler.h"
#include "Networking/MessageHandlers/ActionMessageHandler.h"
#include "Networking/MessageHandlers/ConnectionMessageHandler.h"
#include "Networking/NetworkSpawnManager.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "Core/GameWorld.h"

namespace spelmotor_networking
{

    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createServerDispatcher(
        GameWorld& world,
        NetworkSpawnManager& spawnManager)
    {
        auto dispatcher = std::make_unique<MessageDispatcher>();

        // Server receives actions from clients
        // Note: You'll need a NetworkIdentityRegistry - for now we can skip ActionMessageHandler
        // or create a simple one

        // Server doesn't receive spawn messages (it sends them)
        // But it might receive connection messages

        return dispatcher;
    }

    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createClientDispatcher(
        GameWorld& world,
        NetworkSpawnManager& spawnManager)
    {
        auto dispatcher = std::make_unique<MessageDispatcher>();

        // Client receives spawn messages
        dispatcher->registerMessageHandler(
            MessageTypes::SpawnMessage,
            std::make_unique<SpawnMessageHandler>(spawnManager));

        // Client receives destroy messages
        dispatcher->registerMessageHandler(
            MessageTypes::ObjectDestroyMessage,
            std::make_unique<ObjectDestroyMessageHandler>(spawnManager));

        return dispatcher;
    }

}