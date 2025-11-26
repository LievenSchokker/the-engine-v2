#pragma once


#include "../IMessage.h"
#include "../MessageTypes.h"
#include "Networking/Connection/ConnectionStatus.h"


/**
 * @brief Message used to communicate connection state changes.
 *
 * This message is sent to notify peers of connection status updates
 * such as connecting, connected, or disconnected states.
 */
class ConnectionMessage final : public IMessage
{
public:
    /**
     * @brief Constructs a ConnectionMessage with default status.
     */
    ConnectionMessage();

    /**
     * @brief Returns the message type identifier.
     * @return Always returns MessageTypes::ConnectionMessage.
     */
    MessageTypes getMessageType() const override
    {
        return MessageTypes::ConnectionMessage;
    }

    /**
     * @brief Serializes the message into a byte buffer.
     * @return A vector of bytes representing the serialized message.
     */
    std::vector<std::byte> serialize() const override;

    /**
     * @brief Deserializes a byte buffer into this message.
     * @param data Pointer to the raw byte data.
     * @param length Size of the data in bytes.
     * @return True if deserialization succeeded, false otherwise.
     */
    bool deserialize(const std::byte* data, size_t length) override;

    /**
     * @brief Validates that the message contains well-formed data.
     * @return True if the message is valid, false otherwise.
     */
    bool validate() const override;

    /**
     * @brief Gets the current connection status.
     * @return The ConnectionStatus stored in this message.
     */
    ConnectionStatus getStatus() const;

    /**
     * @brief Sets the connection status.
     * @param newStatus The ConnectionStatus to store.
     */
    void setStatus(ConnectionStatus newStatus);
private:
    ConnectionStatus status;  ///< The connection state this message represents.
};