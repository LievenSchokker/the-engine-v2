//
// Created by samle on 25/11/2025.
//

#pragma once
#include "BaseMessageHandler.h"
#include "Networking/Context/INetworkContext.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"
#include "Networking/Messages/Concretes/SpawnGameObjectMessage.h"

class SpawnGameObjectMessageHandler : public BaseMessageHandler<spelmotor_networking::SpawnGameObjectMessage>
{
    public:
        SpawnGameObjectMessageHandler(ConnectionMode mode,const INetworkContext& networkContext) : BaseMessageHandler(mode, networkContext) {};
        ~SpawnGameObjectMessageHandler() override;

    protected:
        void handleMessageContextBased(const spelmotor_networking::SpawnGameObjectMessage& message) override;
        void handleMessageClient(const spelmotor_networking::SpawnGameObjectMessage &message) override;
        void handleMessageServer(const spelmotor_networking::SpawnGameObjectMessage &message) override;

};
