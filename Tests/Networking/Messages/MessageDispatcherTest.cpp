#include <gtest/gtest.h>
#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/MessageHandlers/IMessageHandler.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"

using namespace spelmotor_networking;

class FakeMessage : public IMessage
{
public:
    explicit FakeMessage(MessageTypes type) : type(type) {}

    [[nodiscard]] MessageTypes getMessageType() const override { return type; }

    [[nodiscard]] std::vector<std::byte> serialize() const override
    {
        return {};
    }

    bool deserialize(const std::byte* data, size_t length) override
    {
        return true;
    }

    [[nodiscard]] bool validate() const override
    {
        return true;
    }

private:
    MessageTypes type;
};

class FakeMessageHandler : public IMessageHandler
{
public:
    int callCount = 0;
    IMessage* lastMessage = nullptr;

    void handleMessage(const std::unique_ptr<IMessage> message) override
    {
        callCount++;
        lastMessage = message.get();
    }
};

class MessageDispatcherTest : public ::testing::Test
{
protected:
    MessageDispatcher dispatcher;
};

TEST_F(MessageDispatcherTest, RegisterHandler_ReturnsTrue_WhenTypeNotRegistered)
{
    //arrange
    auto handler = std::make_unique<FakeMessageHandler>();

    //act
    bool result = dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(handler));

    //assert
    EXPECT_TRUE(result);
}

TEST_F(MessageDispatcherTest, RegisterHandler_ReturnsFalse_WhenTypeAlreadyRegistered)
{
    //arrange
    auto handler1 = std::make_unique<FakeMessageHandler>();
    auto handler2 = std::make_unique<FakeMessageHandler>();

    //act
    dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(handler1));
    bool result = dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(handler2));

    //assert
    EXPECT_FALSE(result);
}

TEST_F(MessageDispatcherTest, RegisterHandler_AllowsMultipleDifferentTypes)
{
    //arrange
    auto handler1 = std::make_unique<FakeMessageHandler>();
    auto handler2 = std::make_unique<FakeMessageHandler>();

    //act
    bool result1 = dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(handler1));
    bool result2 = dispatcher.registerMessageHandler(MessageTypes::ActionMessage, std::move(handler2));

    //assert
    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
}

TEST_F(MessageDispatcherTest, DeregisterHandler_ReturnsTrue_WhenHandlerExists)
{
    //arrange
    auto handler = std::make_unique<FakeMessageHandler>();
    dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(handler));

    //act
    bool result = dispatcher.deregisterMessageHandler(MessageTypes::ConnectionMessage);

    //assert
    EXPECT_TRUE(result);
}

TEST_F(MessageDispatcherTest, DeregisterHandler_ReturnsFalse_WhenHandlerDoesNotExist)
{
    //arrange
    bool result = dispatcher.deregisterMessageHandler(MessageTypes::ConnectionMessage);

    //act

    //assert
    EXPECT_FALSE(result);
}

TEST_F(MessageDispatcherTest, DeregisterHandler_PreventsMessageHandling)
{
    //arrange
    auto handler = std::make_unique<FakeMessageHandler>();
    dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(handler));
    dispatcher.deregisterMessageHandler(MessageTypes::ConnectionMessage);

    //act
    auto message = std::make_unique<FakeMessage>(MessageTypes::ConnectionMessage);
    dispatcher.processMessage(std::move(message));

    auto newHandler = std::make_unique<FakeMessageHandler>();
    auto* newHandlerPtr = newHandler.get();
    dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(newHandler));

    //assert
    EXPECT_EQ(newHandlerPtr->callCount, 0);
}

TEST_F(MessageDispatcherTest, ProcessMessage_HandlesMultipleMessages)
{
    //arrange
    auto handler = std::make_unique<FakeMessageHandler>();
    auto* handlerPtr = handler.get();

    dispatcher.registerMessageHandler(MessageTypes::ConnectionMessage, std::move(handler));

    //act
    dispatcher.processMessage(std::make_unique<FakeMessage>(MessageTypes::ConnectionMessage));
    dispatcher.processMessage(std::make_unique<FakeMessage>(MessageTypes::ConnectionMessage));
    dispatcher.processMessage(std::make_unique<FakeMessage>(MessageTypes::ConnectionMessage));

    //assert
    EXPECT_EQ(handlerPtr->callCount, 3);
}