#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "Networking/Server/Server.h"
#include "Networking/ITransport.h"
#include "Networking/TransportGNS.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/TransportResult.h"

/**
 * @brief Mock transport for testing Server logic in isolation.
 */
class MockTransport : public ITransport
{
public:
    // Track calls for verification
    std::vector<OutgoingRawMessage> sentMessages;
    std::vector<int> disconnectedClients;
    bool socketOpened = false;
    bool socketClosed = false;

    TransportResult setUpListenSocketResult = TransportResult::SUCCESS;
    TransportResult sendResult = TransportResult::SUCCESS;

    TransportResult setUpListenSocket(const uint16_t& port) override
    {
        socketOpened = true;
        return setUpListenSocketResult;
    }

    TransportResult connectByIPAdress(const char* serverAddress, const uint16_t& port) override
    {
        return TransportResult::SUCCESS;
    }

    TransportResult send(const OutgoingRawMessage& message) override
    {
        sentMessages.push_back({message.connectionID,
            std::vector<std::byte>(message.data(), message.data() + message.size()),
            message.sendMode});
        return sendResult;
    }

    TransportResult sendToAll(OutgoingRawMessage& message) override
    {
        return sendResult;
    }

    bool disconnectFromSocket(const int& connectionId) override
    {
        disconnectedClients.push_back(connectionId);
        return true;
    }

    void poll() override {}

    bool closeOpenSocket() override
    {
        socketClosed = true;
        return true;
    }

    // Test helpers to simulate events
    void simulateClientConnected(int clientId)
    {
        Connection conn{ConnectionMode::Client, clientId, ConnectionStatus::Connected};
        if (onConnectionChanged) onConnectionChanged(conn);
    }

    void simulateClientDisconnected(int clientId)
    {
        Connection conn{ConnectionMode::Client, clientId, ConnectionStatus::Death};
        if (onConnectionChanged) onConnectionChanged(conn);
    }

    void simulateMessageReceived(const IncomingRawMessage& message)
    {
        if (onMessageReceived) onMessageReceived(message);
    }
};

/**
 * @brief Server tests using mock transport.
 */
class ServerTest : public ::testing::Test
{
protected:
    MockTransport* mockTransportPtr = nullptr;

    std::unique_ptr<Server> createServerWithMock(uint16_t port)
    {
        auto mockTransport = std::make_unique<MockTransport>();
        mockTransportPtr = mockTransport.get();

        ServerConnectionInformation info{};
        info.port = port;

        return std::make_unique<Server>(info, std::move(mockTransport));
    }
};

// Test 1: Server rejects invalid port
TEST_F(ServerTest, ConstructorThrowsOnZeroPort)
{
    ServerConnectionInformation info{};
    std::unique_ptr<ITransport> transport = std::make_unique<TransportGNS>();
    info.port = 0;

    EXPECT_THROW(Server server(info, std::move(transport)), std::runtime_error);
}

// Test 2: sendMessage fails for unknown client
TEST_F(ServerTest, SendMessageFailsForUnknownClient)
{
    auto server = createServerWithMock(7777);
    server->start();

    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);

    bool result = server->sendMessage(999, message);

    EXPECT_FALSE(result);
}

// Test 3: broadcastMessage succeeds with no clients
TEST_F(ServerTest, BroadcastMessageSucceedsWithNoClients)
{
    auto server = createServerWithMock(7777);
    server->start();

    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);

    bool result = server->broadcastMessage(message);

    EXPECT_TRUE(result);
}

// Test 4: KickClient sends message and disconnects
TEST_F(ServerTest, KickClientSendsDisconnectAndRemovesClient)
{
    auto server = createServerWithMock(7777);
    server->start();

    mockTransportPtr->simulateClientConnected(42);

    server->kickClient(42);

    ASSERT_EQ(mockTransportPtr->sentMessages.size(), 1);
    EXPECT_EQ(mockTransportPtr->sentMessages[0].connectionID, 42);

    ASSERT_EQ(mockTransportPtr->disconnectedClients.size(), 1);
    EXPECT_EQ(mockTransportPtr->disconnectedClients[0], 42);
}