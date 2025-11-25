//
// Created by samle on 25/11/2025.
//
#include "MessageDispatcherFactory.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "../MessageHandlers/IMessageHandler.h"
#include ""

#include "Networking/MessageHandlers/ConnectionMessageHandler.h"

namespace spelmotor_networking
{
    unique_ptr<MessageDispatcher> MessageDispatcherFactory::createMessageDispatcher()
    {
        std::unique_ptr<MessageDispatcher> dispatcher = std::make_unique<MessageDispatcher>();

        /// Register a MessageType with a handler to process the message
        dispatcher->registerMessageHandler(MessageTypes::ConnectionMessage, std::make_unique<ConnectionMessageHandler>());

        return dispatcher;
    }

} // spelmotor_networking