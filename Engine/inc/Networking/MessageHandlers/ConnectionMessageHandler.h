//
// Created by samle on 25/11/2025.
//


#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"


class ConnectionMessageHandler : public BaseMessageHandler<ConnectionMessage>
{
    public:
        explicit ConnectionMessageHandler(ConnectionMode mode)
            : BaseMessageHandler(mode) {};

        ~ConnectionMessageHandler() override = default;

        void handleMessageClient(const ConnectionMessage& msg) override;
        void handleMessageServer(const ConnectionMessage& msg) override;
};
