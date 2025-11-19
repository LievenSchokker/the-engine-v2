// IMessage.h
#pragma once
#include <cstddef>
#include <vector>
#include <cstdint>
#include "MessageTypes.h"

class IMessage
{
public:
    virtual ~IMessage() = default;

    /**
     * @brief Get the type of this message
     */
    virtual MessageTypes getMessageType() const = 0;

    /**
     * @brief Serialize the message payload (WITHOUT the type header)
     * The type header is added by MessageWriter
     */
    virtual std::vector<std::byte> serialize() const = 0;

    /**
     * @brief Deserialize from raw bytes (WITHOUT the type header)
     * The type header is already read by MessageReader
     */
    virtual bool deserialize(const std::byte* data, size_t length) = 0;

    /**
     * @brief Validate the message data
     */
    virtual bool validate() const = 0;
};