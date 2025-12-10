#pragma once

<<<<<<< HEAD
#include <memory>
=======

#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/MessageDispatcher.h"
#include "Scene/SceneManager.h"

class IMessage;
class IMessageHandler;
>>>>>>> origin/development

namespace spelmotor_networking
{
    class MessageDispatcher;
}

class GameWorld;
class NetworkSpawnManager;
class NetworkContext;
class NetworkIdentityRegistry;

namespace spelmotor_networking
{
<<<<<<< HEAD
    class MessageDispatcherFactory
    {
    public:
        static std::unique_ptr<MessageDispatcher> createServerDispatcher(
            GameWorld& world,
            NetworkSpawnManager& spawnManager,
            NetworkContext& context,
            NetworkIdentityRegistry& registry);

        static std::unique_ptr<MessageDispatcher> createClientDispatcher(
            GameWorld& world,
            NetworkSpawnManager& spawnManager,
            NetworkContext& context,
            NetworkIdentityRegistry& registry);
    };
}
=======
class MessageDispatcherFactory
{
   public:
	static std::unique_ptr<MessageDispatcher> createMessageDispatcher(
		ConnectionMode mode, GameWorld& context);
};
}  // namespace spelmotor_networking
>>>>>>> origin/development
