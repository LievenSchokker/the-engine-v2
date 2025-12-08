#pragma once


#include <memory>
#include <vector>
#include <cstdint>

#include "Networking/SendMode.h"


class OutgoingRawMessage;
class IMessage;
enum class SendMode;

class MessageWriter
{
   public:
	/**
	 * @brief Converts an IMessage into an OutgoingRawMessage ready to send.
	 *
	 * @param message The message to serialize
	 * @param connectionId The target connection ID
	 * @param sendMode The send mode (Reliable, Unreliable, etc.)
	 * @return OutgoingRawMessage ready to be sent over the network
	 */
	static OutgoingRawMessage writeMessage(
		const IMessage& message, int connectionId,
		SendMode sendMode = SendMode::ReliableOrdered);

   private:
	/**
	 * @brief Builds a packet with message type header + payload
	 *
	 * Packet format:
	 * [0-1]: Message Type (uint16_t)
	 * [2-N]: Payload (serialized message data)
	 */
	static std::vector<std::byte> buildPacket(const IMessage& message);
};