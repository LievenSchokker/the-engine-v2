//
// Created by samle on 25/11/2025.
//


#pragma once
#include "INetworkContext.h"

class ClientNetworkContext : public INetworkContext
{
    public:
        ClientNetworkContext() = default;
        ~ClientNetworkContext() override;

         bool spawnGameObject(uint32_t netId, Vector2 position) override;
         bool destroyGameObject(uint32_t netId) override;
         bool sendRPC() override;
         bool sendCommand() override;
};
