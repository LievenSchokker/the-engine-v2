#include <gtest/gtest.h>
#include "Core/EngineLoops/ServerLoop.h"
#include "Core/EngineLoops/ClientLoop.h"
#include "Core/ApplicationSpecifications.h"
#include "Game.h"
#include <vector>
#include "Networking/ITransport.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/TransportResult.h"
#include "Networking/SendMode.h"

struct SentMessageRecord
{
    int connectionID;
    std::vector<std::byte> buffer;
    SendMode sendMode;
};

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

class ServerLoopTest : public ::testing::Test
{
protected:
    MockTransport* mockTransportPtr = nullptr;

    void SetUp() override
    {
        ApplicationSpecifications specs;
        specs.networkingOptions.tickRate = 60;
        specs.networkingOptions.port = 7777;

        auto mockTransport = std::make_unique<MockTransport>();
        mockTransportPtr = mockTransport.get();

        std::unique_ptr<Game> game = std::make_unique<Game>();
        game->setApplicationSpecifications(specs);

        auto testScene = std::make_unique<Scene>("TestScene");
        game->addScene(std::move(testScene));

        serverLoop = std::make_unique<ServerLoop>(std::move(game), std::move(mockTransport));
    }

    void TearDown() override
    {
        if (serverLoop)
        {
            serverLoop->shutdown();
        }
    }

    std::unique_ptr<ServerLoop> serverLoop;
};

TEST_F(ServerLoopTest, ConstructionInitializesSubsystems)
{
	// Verify all core subsystems are created and accessible
	EXPECT_NE(serverLoop->getGameWorld(), nullptr);
	EXPECT_NE(serverLoop->getSceneManager(), nullptr);
}

TEST_F(ServerLoopTest, ClockReturnsIncreasingTime)
{
	auto clock = serverLoop->getClock();

	double firstSample = clock();

	// Burn some CPU time to ensure measurable difference
	volatile int sum = 0;
	for (int i = 0; i < 100000; ++i)
	{
		sum += i;
	}

	double secondSample = clock();

	EXPECT_GT(secondSample, firstSample);
}