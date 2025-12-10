#include <gtest/gtest.h>

<<<<<<< HEAD
#include "../../../Engine/inc/Networking/Messages/Concretes/ActionMessage.h"
=======
#include "Networking/Messages/ConcreteMessages/ActionMessage.h"
>>>>>>> origin/development

class ActionMessageTest : public ::testing::Test
{
};


TEST_F(ActionMessageTest, ActionMessageSerializationAndDeserializationValidates)
{
    const auto action = ActionMessage(1000, 100, "WhatDaHellie", 60);

    const std::vector<std::byte> bytes = action.serialize();
    ActionMessage newAction = {};
    newAction.deserialize(bytes.data(), bytes.size());

    EXPECT_EQ(newAction.getAction(), action.getAction());
    EXPECT_EQ(newAction.getComponentIdentity(), action.getComponentIdentity());
    EXPECT_EQ(newAction.getGameObjectIdentity(), action.getGameObjectIdentity());
    EXPECT_EQ(newAction.getTick(), action.getTick());
}