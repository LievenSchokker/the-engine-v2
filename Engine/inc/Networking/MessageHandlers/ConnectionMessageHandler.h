//
// Created by samle on 25/11/2025.
//


#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"


class ConnectionMessageHandler : public BaseMessageHandler<ConnectionMessage>
{
    public:
        explicit ConnectionMessageHandler(ConnectionMode mode, const INetworkContext& networkContext)
            : BaseMessageHandler(mode, networkContext) {};

        ~ConnectionMessageHandler() override;

        void handleMessageContextBased(const ConnectionMessage &message) override;
        void handleMessageClient(const ConnectionMessage& msg) override;
        void handleMessageServer(const ConnectionMessage& msg) override;
};
