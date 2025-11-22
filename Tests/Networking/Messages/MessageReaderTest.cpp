#include <gtest/gtest.h>


#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/OutgoingRawMessage.h"


class MessageReaderTest : public ::testing::Test
{
};

//Test 1:
TEST_F(MessageReaderTest, ReadsConnectionMessageCorrectly)
{
    // Create and serialize a message
    ConnectionMessage original;
    original.setStatus(ConnectionStatus::Connected);
    const std::vector<std::byte> serialized = original.serialize();

    // Create incoming raw message from serialized data
    const IncomingRawMessage raw(1, serialized.data(), serialized.size());

    // Read it back
    const std::unique_ptr<IMessage> result = MessageReader::readMessage(raw);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->getMessageType(), MessageTypes::ConnectionMessage);
}

TEST_F(MessageReaderTest, ReadsConnectionStatusCorrectly)
{
    ConnectionMessage original;
    original.setStatus(ConnectionStatus::Disconnected);
    const std::vector<std::byte> serialized = original.serialize();

    const IncomingRawMessage raw(1, serialized.data(), serialized.size());
    const std::unique_ptr<IMessage> result = MessageReader::readMessage(raw);

    ASSERT_NE(result, nullptr);
    auto* connectionMsg = dynamic_cast<ConnectionMessage*>(result.get());
    ASSERT_NE(connectionMsg, nullptr);
    EXPECT_EQ(connectionMsg->getStatus(), ConnectionStatus::Disconnected);
}

TEST_F(MessageReaderTest, ReturnsNullptrForEmptyData)
{
    const IncomingRawMessage raw(1, nullptr, 0);

    const std::unique_ptr<IMessage> result = MessageReader::readMessage(raw);

    EXPECT_EQ(result, nullptr);
}

TEST_F(MessageReaderTest, ReturnsNullptrForUnknownMessageType)
{
    const std::vector<std::byte> invalidData = { std::byte{255} }; // Invalid message type
    const IncomingRawMessage raw(1, invalidData.data(), invalidData.size());

    const std::unique_ptr<IMessage> result = MessageReader::readMessage(raw);

    EXPECT_EQ(result, nullptr);
}

/**
 * @brief Round-trip test: Write then Read
 */
TEST_F(MessageReaderTest, RoundTripPreservesMessageData)
{
    // Create original message
    ConnectionMessage original;
    original.setStatus(ConnectionStatus::Connecting);
    constexpr int connectionId = 99;

    // Write it
    const OutgoingRawMessage outgoing = MessageWriter::writeMessage(original, connectionId, SendMode::ReliableOrdered);

    // Read it back
    IncomingRawMessage incoming(connectionId, outgoing.data(), outgoing.size());
    std::unique_ptr<IMessage> result = MessageReader::readMessage(incoming);

    // Verify
    ASSERT_NE(result, nullptr);
    auto* connectionMsg = dynamic_cast<ConnectionMessage*>(result.get());
    ASSERT_NE(connectionMsg, nullptr);
    EXPECT_EQ(connectionMsg->getStatus(), ConnectionStatus::Connecting);
}