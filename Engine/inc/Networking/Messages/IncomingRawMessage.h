#pragma once


#include <cstdint>
#include <string_view>


/**
 * @brief A non-owning view of raw network message data.
 *
 * This struct provides a lightweight reference to message data received from
 * the network layer. It does not own the underlying buffer.
 *
 * @warning The pointed-to data is only valid for the duration of the receive
 *          callback. Do not store this struct beyond that scope.
 */
struct IncomingRawMessage
{
    /**
     * @brief Constructs an IncomingRawMessage.
     * @param connectionID The ID of the connection this message was received from.
     * @param dataPointer Pointer to the raw message bytes.
     * @param length Size of the message in bytes.
     */
    IncomingRawMessage(const int connectionID, const std::byte* dataPointer, const size_t length)
        : connectionID(connectionID),
          data(dataPointer),
          length(length)
    {
    }

    int connectionID;       ///< The connection this message originated from.
    const std::byte* data;  ///< Pointer to raw message bytes.
    size_t length;          ///< Size of the message in bytes.
};