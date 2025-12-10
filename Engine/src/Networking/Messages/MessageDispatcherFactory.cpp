#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/MessageHandlers/ActionMessageHandler.h"
#include "Networking/MessageHandlers/ObjectDestroyMessageHandler.h"
#include "Networking/MessageHandlers/SpawnMessageHandler.h"
#include "Networking/MessageHandlers/StateSyncHandler.h"
#include "Networking/MessageHandlers/WelcomeMessageHandler.h"
#include "Networking/Messages/MessageDispatcher.h"

#include <memory>

#include "Networking/NetworkSpawnManager.h"

namespace spelmotor_networking
{

    std::unique_ptr<MessageDispatcher>
    MessageDispatcherFactory::createServerDispatcher(
        GameWorld& world,
        NetworkSpawnManager& spawnManager,
        NetworkIdentityRegistry& registry)
    {
        auto dispatcher = std::make_unique<MessageDispatcher>();

        dispatcher->registerMessageHandler(
            MessageTypes::ActionMessage,
            std::make_unique<ActionMessageHandler>(world, registry));

        return dispatcher;
    }

    std::unique_ptr<MessageDispatcher>
    MessageDispatcherFactory::createClientDispatcher(
        GameWorld& world,
        NetworkSpawnManager& spawnManager,
        NetworkIdentityRegistry& registry)
    {
        auto dispatcher = std::make_unique<MessageDispatcher>();

        dispatcher->registerMessageHandler(
            MessageTypes::WelcomeMessage,
            std::make_unique<WelcomeMessageHandler>(world));

        dispatcher->registerMessageHandler(
            MessageTypes::SpawnMessage,
            std::make_unique<SpawnMessageHandler>(world , spawnManager));

        dispatcher->registerMessageHandler(
            MessageTypes::ObjectDestroyMessage,
            std::make_unique<ObjectDestroyMessageHandler>(world, spawnManager));

        dispatcher->registerMessageHandler(
            MessageTypes::ActionMessage,
            std::make_unique<ActionMessageHandler>(world, registry));

        dispatcher->registerMessageHandler(
            MessageTypes::StateSyncMessage,
            std::make_unique<StateSyncMessageHandler>(world, registry));

        return dispatcher;
    }

}
