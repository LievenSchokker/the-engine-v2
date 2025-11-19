
#include "Networking/Messages/MessageReader.h"

#include "Networking/RawMessage.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/MessageTypes.h"


IMessage& MessageReader::ReadMessage(RawMessage message)
{
    if (message.getLength() < 0)
    {

    }

    if (message.getPayload()[0] == MessageTypes::ConnectionMessage)
    {

    }
    return &ConnectionMessage();
}



uint16_t MessageReader::readMessageType(const RawMessage& message)
{
    const std::byte* data = message.getPayload();

    uint16_t type;
    std::memcpy(&type, data, sizeof(uint16_t));

    return type;
}