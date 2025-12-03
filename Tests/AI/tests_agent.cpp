//
// Created by samle on 03/12/2025.
//

#include <gtest/gtest.h>

#include "TestAgentModule.h"
#include "AI/Agent.h"
#include "GameObject/GameObject.h"


class AgentTest : public ::testing::Test
{
    protected:
        GameObject* gameObject = nullptr;
        Agent* agent = nullptr;

        void SetUp() override
        {
            gameObject = new GameObject();
            agent = gameObject->addComponent<Agent>();

            agent->setMaxModuleForceMagnitude(100);
            agent->setMaxVelocityMagnitude(100);
        }

        void TearDown() override
        {
            delete gameObject;
            gameObject = nullptr;
            agent = nullptr;
        }
};

/// Tests to see if the agent behaviour is enabled under default circumstances
/// Meaning addComponent<agent> has been used on a GO, and Agent::onAwake has been called
TEST_F(AgentTest, AgentIsEnabledAfterAwake)
{
    agent->awake();
    EXPECT_TRUE(agent->getIsEnabled());
}


TEST(AgentTests, AgentIsDisabledAfterAwakeIfTransformIsMissing)
{
    Agent* agent = new Agent();

    EXPECT_EQ(agent->getGameObject(), nullptr);
    EXPECT_EQ(agent->getTransform(), nullptr);

    agent->awake();

    EXPECT_EQ(agent->getIsEnabled(), false);
}

/// Tests if the computeModuleForce() method returns the vector computed by the assigned modules.
TEST_F(AgentTest, ComputeModuleForce)
{
    agent->addAgentModule<TestAgentModule>(1);
    agent->computeModuleForce();

    EXPECT_EQ(agent->computeModuleForce(), Vector2::one());
}

/// Tests if the computeModuleForce method returns a computed vector based on weights.
TEST_F(AgentTest, WeightedComputeModuleForce)
{
    /// Set weight to 5.
    agent->addAgentModule<TestAgentModule>(5);
    agent->computeModuleForce();

    EXPECT_EQ(agent->computeModuleForce(), Vector2(5.0f, 5.0f));
}