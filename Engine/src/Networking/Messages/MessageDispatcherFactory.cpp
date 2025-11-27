#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/MessageHandlers/SpawnMessageHandler.h"
#include "Networking/MessageHandlers/ObjectDestroyMessageHandler.h"
#include "Networking/MessageHandlers/ActionMessageHandler.h"
#include "Networking/MessageHandlers/WelcomeMessageHandler.h"
#include "Networking/NetworkSpawnManager.h"
#include "Networking/NetworkingIdentityRegistry.h"
#include "Core/GameWorld.h"

namespace spelmotor_networking
{

    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createServerDispatcher(
        GameWorld& world,
        NetworkSpawnManager& spawnManager,
        NetworkContext& context,
        NetworkIdentityRegistry& registry)
    {
        auto dispatcher = std::make_unique<MessageDispatcher>();

        dispatcher->registerMessageHandler(
            MessageTypes::ActionMessage,
            std::make_unique<ActionMessageHandler>(context, registry));

        return dispatcher;
    }

    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createClientDispatcher(
        GameWorld& world,
        NetworkSpawnManager& spawnManager,
        NetworkContext& context,
        NetworkIdentityRegistry& registry)
    {
        auto dispatcher = std::make_unique<MessageDispatcher>();

    	//Welcome Message
        dispatcher->registerMessageHandler(
            MessageTypes::WelcomeMessage,
            std::make_unique<WelcomeMessageHandler>(world));

    	//Spawn Message
        dispatcher->registerMessageHandler(
            MessageTypes::SpawnMessage,
            std::make_unique<SpawnMessageHandler>(spawnManager));

        //Object Destroy Message
        dispatcher->registerMessageHandler(
            MessageTypes::ObjectDestroyMessage,
            std::make_unique<ObjectDestroyMessageHandler>(spawnManager));

        //Action Messag
        dispatcher->registerMessageHandler(
            MessageTypes::ActionMessage,
            std::make_unique<ActionMessageHandler>(context, registry));

        return dispatcher;
    }

}