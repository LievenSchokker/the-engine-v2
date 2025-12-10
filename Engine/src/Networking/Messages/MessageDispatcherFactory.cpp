<<<<<<< HEAD

#include "Networking/Messages/MessageDispatcherFactory.h"

#include "Networking/MessageHandlers/ActionMessageHandler.h"
#include "Networking/MessageHandlers/ObjectDestroyMessageHandler.h"
#include "Networking/MessageHandlers/SpawnMessageHandler.h"
#include "Networking/MessageHandlers/StateSyncHandler.h"
#include "Networking/MessageHandlers/WelcomeMessageHandler.h"
#include "Networking/Messages/Concretes/ActionMessage.h"
#include "Networking/Messages/MessageDispatcher.h"

#include <memory>

namespace spelmotor_networking
{
std::unique_ptr<MessageDispatcher>
MessageDispatcherFactory::createServerDispatcher(
	GameWorld& world,
	NetworkSpawnManager& spawnManager,
	NetworkContext& context,
	NetworkIdentityRegistry& registry)
{
	auto dispatcher = std::make_unique<MessageDispatcher>();
=======
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include "Networking/MessageHandlers/IMessageHandler.h"
#include "Networking/MessageHandlers/ConnectionMessageHandler.h"
#include "Scene/SceneManager.h"


namespace spelmotor_networking
{
    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createMessageDispatcher(ConnectionMode mode, GameWorld& context)
    {
        std::unique_ptr<MessageDispatcher> dispatcher = std::make_unique<MessageDispatcher>();
>>>>>>> origin/development

	dispatcher->registerMessageHandler(
	   MessageTypes::ActionMessage,
	   std::make_unique<ActionMessageHandler>(context, registry));

	return dispatcher;
}

<<<<<<< HEAD
std::unique_ptr<MessageDispatcher>
MessageDispatcherFactory::createClientDispatcher(
	GameWorld& world,
	NetworkSpawnManager& spawnManager,
	NetworkContext& context,
	NetworkIdentityRegistry& registry)
{
	auto dispatcher = std::make_unique<MessageDispatcher>();

	dispatcher->registerMessageHandler(
	   MessageTypes::WelcomeMessage,
	   std::make_unique<WelcomeMessageHandler>(world));

	dispatcher->registerMessageHandler(
	   MessageTypes::SpawnMessage,
	   std::make_unique<SpawnMessageHandler>(spawnManager));

	dispatcher->registerMessageHandler(
	   MessageTypes::ObjectDestroyMessage,
	   std::make_unique<ObjectDestroyMessageHandler>(spawnManager));

	dispatcher->registerMessageHandler(
	   MessageTypes::ActionMessage,
	   std::make_unique<ActionMessageHandler>(context, registry));

	// State sync handler
	dispatcher->registerMessageHandler(
	   MessageTypes::StateSyncMessage,
	   std::make_unique<StateSyncMessageHandler>(context, registry));

	return dispatcher;
}
}
=======
>>>>>>> origin/development
