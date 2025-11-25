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
    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createMessageDispatcher(ConnectionMode mode, const INetworkContext& context)
    {
        std::unique_ptr<MessageDispatcher> dispatcher = std::make_unique<MessageDispatcher>();
        dispatcher->registerMessageHandler(MessageTypes::ConnectionMessage,std::make_unique<ConnectionMessageHandler>(mode, context));

        return dispatcher;
    }
}
// spelmotor_networking
