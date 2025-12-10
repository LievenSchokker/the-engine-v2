#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/IMessage.h"
#include "../../../inc/Networking/Messages/ConcreteMessages/ConnectionMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"


#include <cstring>


OutgoingRawMessage MessageWriter::writeMessage(
    const IMessage& message,
    int connectionId,
    SendMode sendMode)
{
    std::vector<std::byte> payload = message.serialize();

    std::vector<std::byte> buffer(sizeof(uint8_t) + payload.size());
    const auto typeValue = static_cast<uint8_t>(message.getMessageType());
    std::memcpy(buffer.data(), &typeValue, sizeof(uint8_t));
    std::memcpy(buffer.data() + sizeof(uint8_t), payload.data(), payload.size());

    return {connectionId, std::move(buffer), sendMode};
}