#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/MessageHandlers/IMessageHandler.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"


namespace spelmotor_networking
{
    void MessageDispatcher::processMessage(std::unique_ptr<IMessage> message)
    {
        IMessageHandler* handler = getMessageHandler(message->getMessageType());

        if (handler != nullptr)
        {
            handler->handleMessage(std::move(message));
        }
    }

    bool MessageDispatcher::registerMessageHandler(MessageTypes type, std::unique_ptr<IMessageHandler> handler)
    {
        auto result = messageHandlers.emplace(type, std::move(handler));
        return result.second;
    }


    bool MessageDispatcher::deregisterMessageHandler(MessageTypes type)
    {
        return messageHandlers.erase(type) > 0;
    }


    IMessageHandler* MessageDispatcher::getMessageHandler(MessageTypes type)
    {
        const auto messagehHandler = messageHandlers.find(type);
        return messagehHandler != messageHandlers.end() ? messagehHandler->second.get() : nullptr;
    }
}