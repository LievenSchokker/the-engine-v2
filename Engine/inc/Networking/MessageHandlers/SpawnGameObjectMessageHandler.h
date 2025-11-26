//
// Created by samle on 25/11/2025.
//

#pragma once
#include "BaseMessageHandler.h"
#include "Networking/Context/NetworkContext.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"
#include "Networking/Messages/Concretes/SpawnGameObjectMessage.h"

class SpawnGameObjectMessageHandler : public BaseMessageHandler<spelmotor_networking::SpawnGameObjectMessage>
{
    public:
        explicit SpawnGameObjectMessageHandler(NetworkContext& networkContext) : BaseMessageHandler(networkContext) {};
        ~SpawnGameObjectMessageHandler() override;

        void handleMessage(const IMessage &message) override;
};
