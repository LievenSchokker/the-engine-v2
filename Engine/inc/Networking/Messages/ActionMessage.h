#pragma once
#include "IMessage.h"

class ActionMessage final : IMessage
{
	/**
	 * @brief Constructs a ConnectionMessage with default status.
	 */
	ActionMessage();

	/**
	 * @brief Returns the message type identifier.
	 * @return Always returns MessageTypes::ConnectionMessage.
	 */
	MessageTypes getMessageType() const override;
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
};