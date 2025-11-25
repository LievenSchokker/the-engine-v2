//
// Created by samle on 25/11/2025.
//
#include "../../../inc/Networking/Messages/MessageDispatcherFactory.h"
#include "../../../inc/Networking/Messages/MessageDispatcher.h"
#include "../../../inc/Networking/Messages/MessageTypes.h"
#include "../../../inc/Networking/MessageHandlers/IMessageHandler.h"

#include "Networking/MessageHandlers/ConnectionMessageHandler.h"

namespace spelmotor_networking
{
    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createMessageDispatcher()
    {
        std::unique_ptr<MessageDispatcher> dispatcher = std::make_unique<MessageDispatcher>();

        /// Register a MessageType with a handler to process the message
        dispatcher->registerMessageHandler(MessageTypes::ConnectionMessage, std::make_unique<ConnectionMessageHandler>());

        return dispatcher;
    }

} // spelmotor_networking