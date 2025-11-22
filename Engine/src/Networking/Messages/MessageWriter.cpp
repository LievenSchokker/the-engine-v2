// MessageWriter.cpp
#include "Networking/Messages/MessageWriter.h"

#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"
#include <cstring>

OutgoingRawMessage MessageWriter::writeMessage(
    const IMessage& message,
    int connectionId,
    SendMode sendMode)
{
    std::vector<std::byte> packet = buildPacket(message);
    return {connectionId, std::move(packet), sendMode};
}

std::vector<std::byte> MessageWriter::buildPacket(const IMessage& message)
{
    MessageTypes messageType = message.getMessageType();
    auto typeValue = static_cast<uint8_t>(messageType);


    std::vector<std::byte> payload = message.serialize();

    std::vector<std::byte> packet(sizeof(uint8_t) + payload.size());

    std::memcpy(packet.data(), &typeValue, sizeof(uint8_t));

    if (!payload.empty())
    {
        std::memcpy(packet.data() + sizeof(uint8_t),
                    payload.data(),
                    payload.size());
    }

    return packet;
}
