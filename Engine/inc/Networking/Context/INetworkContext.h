//
// Created by samle on 25/11/2025.
//


#pragma once
#include <cstdint>
class ISceneStrategy;
struct Vector2;

class INetworkContext
{
    public:
        /// old.
        virtual ~INetworkContext() = 0 {};
        virtual bool spawnGameObject(uint32_t netId, Vector2 position) = 0;
        virtual bool destroyGameObject(uint32_t netId) = 0;
        virtual bool sendRPC() = 0;
        virtual bool sendCommand() = 0;

        /// new.
        virtual ISceneStrategy& getSceneStrategy() = 0;
};
