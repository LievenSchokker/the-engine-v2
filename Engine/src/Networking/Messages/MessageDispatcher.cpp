//
// Created by samle on 24/11/2025.
//


#include "Networking/Messages/MessageDispatcher.h"

#include "Networking/MessageHandlers/IMessageHandler.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/MessageTypes.h"


namespace spelmotor_networking
{
void MessageDispatcher::processMessage(const IMessage& message)
{
	IMessageHandler* handler = getMessageHandler(message.getMessageType());

	if (handler != nullptr) {
		handler->handle(message);
	}
}

bool MessageDispatcher::registerMessageHandler(MessageTypes type,
                                               std::unique_ptr<IMessageHandler>
                                               handler)
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
	auto it = messageHandlers.find(type);
	return it != messageHandlers.end() ? it->second.get() : nullptr;
}
}