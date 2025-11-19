// MessageReader.cpp (FIXED)
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/IncommingRawMessage.h"
#include "Networking/Messages/MessageTypes.h"
#include <iostream>

std::unique_ptr<IMessage> MessageReader::readMessage(const IncomingRawMessage rawMessage)
{
    if (rawMessage.length < sizeof(uint8_t))
    {
        std::cerr << "[MessageReader] Message too short\n";
        return nullptr;
    }

    MessageTypes messageType = readMessageType(rawMessage);

    std::unique_ptr<IMessage> message = createMessage(messageType);

    if (message == nullptr)
    {
        std::cerr << "[MessageReader] Unknown message type: "
                  << static_cast<int>(messageType) << "\n";
        return nullptr;
    }

    const std::byte* payloadData = rawMessage.data + sizeof(uint8_t);
    size_t payloadLength = rawMessage.length - sizeof(uint8_t);

    if (message->deserialize(payloadData, payloadLength))
    {
        return std::move(message);
    }
    else
    {
        std::cerr << "[MessageReader] Failed to deserialize message type: "
                  << static_cast<int>(messageType) << "\n";
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

MessageTypes MessageReader::readMessageType(const IncomingRawMessage& message)
{
    const std::byte* data = message.data;
    MessageTypes messageType;
    std::memcpy(&messageType, data, sizeof(uint8_t));
    return messageType;
}