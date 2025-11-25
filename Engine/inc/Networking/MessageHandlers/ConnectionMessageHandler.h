//
// Created by samle on 25/11/2025.
//


#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"


class ConnectionMessageHandler : public BaseMessageHandler<ConnectionMessage>
{
    public:
        ConnectionMessageHandler() = default;
        ~ConnectionMessageHandler() override = default;

        void handleMessage(const ConnectionMessage& msg) override;
};
