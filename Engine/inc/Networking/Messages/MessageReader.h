#pragma once

#include "IncomingRawMessage.h"
#include "ConcreteMessages/MessageTypes.h"


#include <memory>


class RawMessage;
class IMessage;


/**
 * @brief Deserializes raw network data into typed message objects.
 *
 * This class provides static methods to parse incoming raw bytes and
 * construct the appropriate IMessage object based on the message header.
 */
class MessageReader
{
public:
    /**
     * @brief Parses a raw message and constructs the corresponding typed message.
     *
     * Reads the message header to determine the type, instantiates the appropriate
     * IMessage subclass, and deserializes the payload into it.
     *
     * @param rawMessage The incoming raw message to parse.
     * @return A unique_ptr to the deserialized message, or nullptr if parsing fails
     *         or the message type is unknown.
     */
    static std::unique_ptr<IMessage> readMessage(const IncomingRawMessage& rawMessage);

private:
    /**
     * @brief Factory method that instantiates a message object by type.
     * @param messageType The type of message to create.
     * @return A unique_ptr to a new IMessage subclass instance, or nullptr
     *         if the type is unrecognized.
     */
    static std::unique_ptr<IMessage> createMessage(MessageTypes messageType);

    /**
     * @brief Extracts the message type from the raw message header.
     * @param message The raw message to read the header from.
     * @return The MessageTypes value parsed from the header.
     */
    static MessageTypes readMessageHeader(const IncomingRawMessage& message);
};