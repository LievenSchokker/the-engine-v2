#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include "Networking/Messages/ConcreteMessages/ConnectionMessage.h"

#include <iostream>
#include <cstring>

#include "Networking/Messages/ConcreteMessages/ActionMessage.h"
#include "Networking/Messages/ConcreteMessages/ObjectDestroyMessage.h"
#include "Networking/Messages/ConcreteMessages/SpawnMessage.h"
#include "Networking/Messages/ConcreteMessages/StateSyncMessage.h"
#include "Networking/Messages/ConcreteMessages/WelcomeMessage.h"

std::unique_ptr<IMessage> MessageReader::readMessage(
	const IncomingRawMessage& rawMessage)
{
	if (rawMessage.length < sizeof(uint8_t))
	{
		return nullptr;
	}

	const MessageTypes messageType = readMessageHeader(rawMessage);

	std::unique_ptr<IMessage> message = createMessage(messageType);

	if (message == nullptr)
	{
		return nullptr;
	}

	const std::byte* payloadData = rawMessage.data + sizeof(uint8_t);
	size_t payloadLength = rawMessage.length - sizeof(uint8_t);

	if (message->deserialize(payloadData, payloadLength))
	{
		return message;
	}
	else
	{
		return nullptr;
	}
}

std::unique_ptr<IMessage> MessageReader::createMessage(MessageTypes messageType)
{
	std::unique_ptr<IMessage> message;

	switch (messageType)
	{
		case MessageTypes::StateSyncMessage:
			message = std::make_unique<StateSyncMessage>();
			break;
		case MessageTypes::ConnectionMessage:
			message = std::make_unique<ConnectionMessage>();
			break;
		case MessageTypes::WelcomeMessage:
			message = std::make_unique<WelcomeMessage>();
			break;
		case MessageTypes::ActionMessage:
			message = std::make_unique<ActionMessage>();
			break;
		case MessageTypes::ObjectDestroyMessage:
			message = std::make_unique<ObjectDestroyMessage>();
			break;
		case MessageTypes::SpawnMessage:
			message = std::make_unique<SpawnMessage>();
			break;
		default:
			return nullptr;
	}

	return message;
}

MessageTypes MessageReader::readMessageHeader(const IncomingRawMessage& message)
{
	const std::byte* data = message.data;
	MessageTypes messageType;
	std::memcpy(&messageType, data, sizeof(uint8_t));
	return messageType;
}