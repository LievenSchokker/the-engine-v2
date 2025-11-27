#pragma once

#include <memory>

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