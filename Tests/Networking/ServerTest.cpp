#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/ITransport.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/TransportResult.h"
#include "Networking/SendMode.h"

/**
 * @brief Record of a sent message for test verification.
 */
struct SentMessageRecord
{
    int connectionID;
    std::vector<std::byte> buffer;
    SendMode sendMode;
};

/**
 * @brief Mock transport for testing Server and Client logic in isolation.
 */
class MockTransport : public ITransport
{
public:
    std::vector<SentMessageRecord> sentMessages;
    std::vector<int> disconnectedClients;
    bool socketOpened = false;
    bool socketClosed = false;

    TransportResult setUpListenSocketResult = TransportResult::SUCCESS;
    TransportResult connectResult = TransportResult::SUCCESS;
    TransportResult sendResult = TransportResult::SUCCESS;

    TransportResult setUpListenSocket(const uint16_t& port) override
    {
        socketOpened = true;
        return setUpListenSocketResult;
    }

    TransportResult connectByIPAdress(const char* serverAddress, const uint16_t& port) override
    {
        return connectResult;
    }

    TransportResult send(const OutgoingRawMessage& message) override
    {
        sentMessages.push_back({
            message.connectionID,
            std::vector<std::byte>(message.data(), message.data() + message.size()),
            message.sendMode
        });
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

    void simulateClientConnected(int clientId)
    {
        Connection conn{ConnectionMode::Client, clientId, ConnectionStatus::Connected};
        if (onConnectionChanged) onConnectionChanged(conn);
    }

    void simulateClientDisconnected(int clientId)
    {
        Connection conn{ConnectionMode::Client, clientId, ConnectionStatus::Terminated};
        if (onConnectionChanged) onConnectionChanged(conn);
    }

    void simulateMessageReceived(const IncomingRawMessage& message)
    {
        if (onMessageReceived) onMessageReceived(message);
    }
};


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

TEST_F(ServerTest, ConstructorThrowsOnZeroPort)
{
    ServerConnectionInformation info{};
    auto transport = std::make_unique<MockTransport>();
    info.port = 0;

    EXPECT_THROW(Server server(info, std::move(transport)), std::runtime_error);
}

TEST_F(ServerTest, SendMessageFailsForUnknownClient)
{
    auto server = createServerWithMock(7777);
    server->start();

    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);

    bool result = server->sendMessage(999, message);

    EXPECT_FALSE(result);
}

TEST_F(ServerTest, BroadcastMessageSucceedsWithNoClients)
{
    auto server = createServerWithMock(7777);
    server->start();

    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);

    bool result = server->broadcastMessage(message);

    EXPECT_TRUE(result);
}

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


class ClientTest : public ::testing::Test
{
protected:
    MockTransport* mockTransportPtr = nullptr;

    std::unique_ptr<Client> createClientWithMock()
    {
        auto mockTransport = std::make_unique<MockTransport>();
        mockTransportPtr = mockTransport.get();

        return std::make_unique<Client>(std::move(mockTransport));
    }
};

TEST_F(ClientTest, SendMessageFailsWhenNotConnected)
{
    auto client = createClientWithMock();

    ConnectionMessage message;
    message.setStatus(ConnectionStatus::Connected);

    bool result = client->sendMessage(message);

    EXPECT_FALSE(result);
}

TEST_F(ClientTest, IsConnectedReturnsFalseInitially)
{
    auto client = createClientWithMock();

    EXPECT_FALSE(client->isConnected());
}

TEST_F(ClientTest, IsConnectedReturnsTrueAfterConnection)
{
    auto client = createClientWithMock();

    mockTransportPtr->simulateClientConnected(1);

    EXPECT_TRUE(client->isConnected());
}

TEST_F(ClientTest, DisconnectClosesSocket)
{
    auto client = createClientWithMock();

    mockTransportPtr->simulateClientConnected(1);
    client->disconnect();

    EXPECT_TRUE(mockTransportPtr->socketClosed);
    EXPECT_FALSE(client->isConnected());
}