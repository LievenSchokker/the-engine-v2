//
// Created by samle on 25/11/2025.
//

#pragma once
#include <cstdint>

#include "INetworkContext.h"
struct Vector2;
class SceneManager;

class ServerNetworkContext : public INetworkContext
{
    public:
        ServerNetworkContext() = default;
        ~ServerNetworkContext() override;

        bool spawnGameObject(uint32_t netId, Vector2 position) override;
        bool destroyGameObject(uint32_t netId) override;
        bool sendRPC() override;
        bool sendCommand() override;

        ISceneStrategy& getSceneStrategy() override;

    private:
        const SceneManager* sceneManager = nullptr;
        ISceneStrategy* sceneStrategy = nullptr;
};
