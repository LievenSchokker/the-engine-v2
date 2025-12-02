#pragma once


#include <vector>
#include <cstdint>
#include "Networking/SendMode.h"


/**
 * @brief An owning container for outgoing network message data.
 *
 * This class takes ownership of the message payload, ensuring the data
 * remains valid until transmission completes. This is necessary for
 * asynchronous sending where the message may outlive its original source.
 */
class OutgoingRawMessage
{
public:
    /**
     * @brief Constructs an OutgoingRawMessage.
     * @param connectionID The ID of the connection to send this message on.
     * @param payload The message data (ownership is transferred via move).
     * @param mode The send mode determining reliability and ordering guarantees.
     */
    OutgoingRawMessage(int connectionID, std::vector<std::byte> payload,
                       SendMode mode = SendMode::ReliableOrdered)
        : connectionID(connectionID)
          , buffer(std::move(payload))
          , sendMode(mode)
    {
    }

    /**
     * @brief Returns a pointer to the raw message bytes.
     * @return Pointer to the beginning of the buffer.
     */
    const std::byte* data() const
    {
        return buffer.data();
    }

    /**
     * @brief Returns the size of the message payload.
     * @return Size in bytes.
     */
    size_t size() const
    {
        return buffer.size();
    }


    int connectionID;                   ///< The target connection for this message.
    const std::vector<std::byte> buffer; ///< The owned message payload.
    const SendMode sendMode;            ///< Reliability and ordering mode.
};