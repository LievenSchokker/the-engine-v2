//
// Created by thijs on 14-11-2025.
//

#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include "SendMode.h"


/**
 * @class RawMessage
 * @brief Represents a raw network message with payload, connection ID, and send mode.
 *
 * This class supports construction from raw bytes or strings and allows retrieval
 * and modification of all message properties.
 */
class RawMessage
{
public:
    /**
     * @brief Construct a RawMessage from raw memory.
     * @param cid The connection ID associated with this message.
     * @param data Pointer to the raw byte data.
     * @param len Length of the data pointed to by @p data.
     * @param sendMode The send mode for transmission (Reliable, Unreliable, etc.).
     */
    RawMessage(int cid, const std::byte* data, size_t len,
               SendMode sendMode = SendMode::Unreliable);

    /**
     * @brief Construct a RawMessage from a string.
     * @param cid The connection ID associated with this message.
     * @param str The payload string.
     * @param sendMode The send mode for transmission.
     */
    RawMessage(int cid, const std::string& str,
               SendMode sendMode = SendMode::ReliableOrdered);

    /**
     * @brief Converts the internal payload to a std::string.
     * @return The payload interpreted as a string.
     */
    std::string toString() const;

    /**
     * @brief Retrieves the connection ID.
     * @return The associated connection ID.
     */
    int getConnectionID() const;

    /**
     * @brief Retrieves the message payload as a vector of bytes.
     * @return The payload.
     */
    std::vector<std::byte> getPayload() const;

    /**
     * @brief Retrieves the send mode of the message.
     * @return The current SendMode.
     */
    SendMode getSendMode() const;

    /**
     * @brief Retrieves the message type.
     * @return A 16-bit message type identifier.
     */
    uint16_t getMessageType() const;

    /**
     * @brief Retrieves the payload length.
     * @return The payload length in bytes.
     */
    size_t getLength() const;


    /**
     * @brief Sets the connection ID.
     * @param cid New connection ID.
     */
    void setConnectionID(int cid);

    /**
     * @brief Sets the payload using raw byte data.
     * @param data Pointer to the raw byte buffer.
     * @param len Number of bytes to copy.
     */
    void setPayload(const std::byte* data, size_t len);

    /**
     * @brief Sets the payload using a std::vector of bytes.
     * @param newPayload The new payload vector.
     */
    void setPayload(const std::vector<std::byte>& newPayload);

    /**
     * @brief Sets the payload using a string.
     * @param str The new payload string.
     */
    void setPayload(const std::string& str);

    /**
     * @brief Sets the send mode.
     * @param mode The new SendMode value.
     */
    void setSendMode(SendMode mode);

    /**
     * @brief Sets the message type.
     * @param type The new 16-bit message type identifier.
     */
    void setMessageType(uint16_t type);

private:
    int connectionID = -1; ///< Connection identifier.
    std::vector<std::byte> payload; ///< Raw message payload.
    SendMode sendMode = SendMode::ReliableOrdered; ///< Transmission mode.
    uint16_t messageType = 0; ///< 16-bit message type identifier.
    size_t length = 0; ///< Cached payload length.
};
