#pragma once


#include <gtest/gtest.h>


#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"


class MessageWriterTest : public ::testing::Test
{
};

//Test 1: Message contains connectionId
TEST_F(MessageWriterTest, WritesConnectionMessageWithCorrectConnectionId)
{
    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);
    constexpr int connectionId = 42;

    const OutgoingRawMessage raw = MessageWriter::writeMessage(message, connectionId, SendMode::ReliableOrdered);


    EXPECT_EQ(raw.connectionID, connectionId);
}

//Test 2: Message contains correct SendMode
TEST_F(MessageWriterTest, WritesConnectionMessageWithCorrectSendMode)
{
    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);

    const OutgoingRawMessage raw = MessageWriter::writeMessage(message, 1, SendMode::Unreliable);


    EXPECT_EQ(raw.sendMode, SendMode::Unreliable);
}

//Test 3: Writer writes empty message
TEST_F(MessageWriterTest, WritesNonEmptyBuffer)
{
    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);


    const OutgoingRawMessage raw = MessageWriter::writeMessage(message, 1, SendMode::ReliableOrdered);


    EXPECT_GT(raw.size(), 0);
    EXPECT_NE(raw.data(), nullptr);
}

