#pragma once

#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/MessageTypes.h"
#include <cstdint>

/**
 * @brief Message sent when a networked object is despawned/destroyed.
 */
class ObjectDestroyMessage final : public IMessage
{
public:
    ObjectDestroyMessage() = default;

    MessageTypes getMessageType() const override
    {
        return MessageTypes::ObjectDestroyMessage;
    }

    std::vector<std::byte> serialize() const override;
    bool deserialize(const std::byte* data, size_t length) override;
    bool validate() const override;

    uint32_t netId = 0; ///< Network ID of object to destroy
};
