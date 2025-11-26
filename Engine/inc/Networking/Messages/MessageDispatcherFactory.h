#pragma once

#include <memory>

class GameWorld;
class NetworkSpawnManager;

namespace spelmotor_networking
{
    class MessageDispatcher;

    class MessageDispatcherFactory
    {
    public:
        static std::unique_ptr<MessageDispatcher> createServerDispatcher(
            GameWorld& world,
            NetworkSpawnManager& spawnManager);

        static std::unique_ptr<MessageDispatcher> createClientDispatcher(
            GameWorld& world,
            NetworkSpawnManager& spawnManager);
    };
}