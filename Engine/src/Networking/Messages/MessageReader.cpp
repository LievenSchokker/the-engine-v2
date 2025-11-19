#include "Networking/Messages/MessageReader.h"

#include <gtest/gtest-message.h>
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/IncommingRawMessage.h"
#include "Networking/Messages/MessageTypes.h"


std::unique_ptr<IMessage> MessageReader::readMessage(const IncomingRawMessage rawMessage)
{
    if (rawMessage.length < sizeof(uint8_t))
    {
        return nullptr;
    }


    std::unique_ptr<IMessage> message = createMessage(rawMessage);
    if (message.get() == nullptr) return nullptr;

    if (message->deserialize(rawMessage.data, rawMessage.length))
    {
        return message;
    }
    else
    {
        return nullptr;
    }
}


std::unique_ptr<IMessage> MessageReader::createMessage(const IncomingRawMessage rawMessage)
{
    MessageTypes messageType = readMessageType(rawMessage);
    std::unique_ptr<IMessage> message;

    switch (messageType)
    {
    case MessageTypes::ConnectionMessage:
        message = std::make_unique<ConnectionMessage>();
        break;
    }

    return nullptr;
}


MessageTypes MessageReader::readMessageType(const IncomingRawMessage& message)
{
    const std::byte* data = message.data;
    MessageTypes messageType;
    std::memcpy(&messageType, data, sizeof(uint8_t));
    return messageType;
}
