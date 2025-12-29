#pragma once


#include "Networking/MessageHandlers/SpawnMessageHandler.h"

#include <memory>

namespace spelmotorNetworking
{
    class MessageDispatcher;
}

class GameWorld;
class NetworkSpawnManager;
class NetworkContext;
class NetworkIdentityRegistry;

namespace spelmotorNetworking
{
    /**
     * @brief Factory for creating role-specific message dispatchers.
     *
     * A Static Factory class that handles creation of a dispatcher.
     * This class defines which routing is allowed within the server
     * and client and how the dispatcher should route the messages.
     *
     */
    class MessageDispatcherFactory
    {
    public:
        /**
         * @brief Creates a dispatcher configured for server authority.
         *
         */
        static std::unique_ptr<MessageDispatcher> createServerDispatcher(
            GameWorld& world,
            NetworkSpawnManager& spawnManager,
            NetworkIdentityRegistry& registry);

        /**
         * @brief Creates a dispatcher configured for client reception.
         *
         */
        static std::unique_ptr<MessageDispatcher> createClientDispatcher(
            GameWorld& world,
            NetworkSpawnManager& spawnManager,
            NetworkIdentityRegistry& registry);
    };
}