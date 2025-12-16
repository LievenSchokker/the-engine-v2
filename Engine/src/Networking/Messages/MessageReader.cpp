#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include "Networking/Messages/ConcreteMessages/ConnectionMessage.h"

#include <iostream>
#include <cstring>

std::unique_ptr<IMessage> MessageReader::readMessage(const IncomingRawMessage rawMessage)
{
    if (rawMessage.length < sizeof(uint8_t))
    {
        return nullptr;
    }

    MessageTypes messageType = readMessageHeader(rawMessage);

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
        case MessageTypes::ConnectionMessage:
            message = std::make_unique<ConnectionMessage>();
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