#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include "Networking/MessageHandlers/IMessageHandler.h"
#include "Networking/MessageHandlers/ConnectionMessageHandler.h"
#include "Scene/SceneManager.h"


namespace spelmotor_networking
{
    std::unique_ptr<MessageDispatcher> MessageDispatcherFactory::createMessageDispatcher(ConnectionMode mode, GameWorld& context)
    {
        std::unique_ptr<MessageDispatcher> dispatcher = std::make_unique<MessageDispatcher>();

        switch (mode)
        {
        case ConnectionMode::Client:
            dispatcher->registerMessageHandler(MessageTypes::ConnectionMessage,std::make_unique<ConnectionMessageHandler>(mode, context));
            /// Add more handlers here...
        case ConnectionMode::Host:
            dispatcher->registerMessageHandler(MessageTypes::ConnectionMessage,std::make_unique<ConnectionMessageHandler>(mode, context));
            /// Add more handlers here...
        default:
            break;
        }

        return dispatcher;
    }
}