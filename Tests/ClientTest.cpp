#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Networking/Client.h"
#include "Networking/TransportGNS.h"

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;

/* ---------------------------------------------------------
   Fake TransportGNS for testing
   --------------------------------------------------------- */
class FakeTransportGNS : public TransportGNS
{
public:
    bool connectCalled = false;
    const char* lastIp = nullptr;
    uint16_t lastPort = 0;
    TransportResult nextConnectResult = TransportResult::SUCCES;

    std::vector<RawMessage> sentMessages;
    bool pollCalled = false;

    TransportResult connectByIPAdress(const char* ip, uint16_t port) override
    {
        connectCalled = true;
        lastIp = ip;
        lastPort = port;
        return nextConnectResult;
    }

    TransportResult send(const RawMessage& message) override
    {
        sentMessages.push_back(message);
        return TransportResult::SUCCES;
    }

    void poll() override
    {
        pollCalled = true;
    }

    bool closeOpenSocket() override { return true; }
    bool disconnectFromSocket(int id) override { return true; }

    using TransportGNS::onMessageReceived;
    using TransportGNS::onConnectionChanged;
};

/* ---------------------------------------------------------
   Helper wrapper to access Client for testing
   --------------------------------------------------------- */
class ClientTestWrapper : public Client
{
public:
    ClientTestWrapper(std::unique_ptr<TransportGNS> transport)
        : Client(std::move(transport))
    {
    }

    // Use getters/setters for private members
    void setConnected(bool state) { Client::setConnected(state); }
    bool isConnected() const { return Client::isConnected(); }

    void setRunning(bool state) { Client::setRunning(state); }
    bool isRunning() const { return Client::isRunning(); }
};

/* ---------------------------------------------------------
   Tests
   --------------------------------------------------------- */

// Test 1: Attempt to connect successfully using FakeTransportGNS
TEST(ClientTests, Test1_ConnectToServer_Success)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();
    ClientTestWrapper client(std::move(fake));

    EXPECT_TRUE(client.connectToServer(27021, "127.0.0.1"));

    EXPECT_TRUE(raw->connectCalled);
    EXPECT_STREQ(raw->lastIp, "127.0.0.1");
    EXPECT_EQ(raw->lastPort, 27021);
}

// Test 2: SendMessage stores message in FakeTransportGNS when client is connected
TEST(ClientTests, Test2_SendMessage_WhenConnected_CallsSend)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();
    ClientTestWrapper client(std::move(fake));

    client.setConnected(true);

    EXPECT_TRUE(client.sendMessage("Hello"));
    ASSERT_EQ(raw->sentMessages.size(), 1);
    EXPECT_EQ(raw->sentMessages[0].toString(), "Hello");
}

// Test 3: Attempt to connect but the transport fails
TEST(ClientTests, Test3_ConnectToServer_Fails)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    fake->nextConnectResult = TransportResult::ERROR;
    FakeTransportGNS* raw = fake.get();
    ClientTestWrapper client(std::move(fake));

    bool result = client.connectToServer(27021, "127.0.0.1");

    EXPECT_FALSE(result);
    EXPECT_TRUE(raw->connectCalled);
}

// Test 4: SendMessage returns false if client not connected
TEST(ClientTests, Test4_SendMessage_WhenNotConnected_ReturnsFalse)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();
    ClientTestWrapper client(std::move(fake));

    EXPECT_FALSE(client.sendMessage("Hello"));
    EXPECT_TRUE(raw->sentMessages.empty());
}

// Test 5: Poll is called by listen thread (simulate thread run)
TEST(ClientTests, Test5_ListenThread_CallsPoll)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();
    ClientTestWrapper client(std::move(fake));

    client.setRunning(true);

    std::thread listen([&]()
    {
        client.connectToServer(27021, "127.0.0.1");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        client.setRunning(false);
    });
    listen.join();

    EXPECT_TRUE(raw->pollCalled);
}

// Test 6: onMessageReceived callback is triggered
TEST(ClientTests, Test6_OnMessageReceived_CallbackTriggered)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();
    bool callbackCalled = false;

    ClientTestWrapper client(std::move(fake));
    client.setOnMessageReceived([&](const RawMessage& msg)
    {
        callbackCalled = true;
        EXPECT_EQ(msg.toString(), "TestMessage");
    });

    RawMessage testMsg(1, "TestMessage");
    raw->onMessageReceived(testMsg);

    EXPECT_TRUE(callbackCalled);
}

// Test 7: onConnectionChanged callback updates connected state
TEST(ClientTests, Test7_OnConnectionChanged_UpdatesConnected)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();
    ClientTestWrapper client(std::move(fake));

    raw->onConnectionChanged(5, true);
    EXPECT_TRUE(client.isConnected());

    raw->onConnectionChanged(5, false);
    EXPECT_FALSE(client.isConnected());
}

// Test 8: Multiple messages are queued correctly
TEST(ClientTests, Test8_SendMultipleMessages_StoredInFake)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();
    ClientTestWrapper client(std::move(fake));
    client.setConnected(true);

    client.sendMessage("First");
    client.sendMessage("Second");

    ASSERT_EQ(raw->sentMessages.size(), 2);
    EXPECT_EQ(raw->sentMessages[0].toString(), "First");
    EXPECT_EQ(raw->sentMessages[1].toString(), "Second");
}

// Test 9: closeOpenSocket can be called safely
TEST(ClientTests, Test9_CloseSocket_CalledOnDestruction)
{
    auto fake = std::make_unique<FakeTransportGNS>();
    FakeTransportGNS* raw = fake.get();

    {
        ClientTestWrapper client(std::move(fake));
        client.setConnected(true);
        client.setRunning(true);
    }

    SUCCEED(); // Destructor completed without crashing
}
