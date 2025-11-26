//
// Created by samle on 25/11/2025.
//


#pragma once

#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Context/NetworkContext.h"
class IMessage;
class IMessageHandler;


namespace spelmotor_networking
{
    class MessageDispatcherFactory
    {
        public:
            static std::unique_ptr<MessageDispatcher> createMessageDispatcher(ConnectionMode connectionMode, NetworkContext& context);
    };
} // spelmotor_networking
