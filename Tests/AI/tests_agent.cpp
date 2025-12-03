//
// Created by samle on 03/12/2025.
//

#include <gtest/gtest.h>

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
            agent->awake();
        }

        void TearDown() override
        {
            delete gameObject;
            gameObject = nullptr;
            agent = nullptr;
        }
};

TEST_F(AgentTest, AwakeSetsTransform)
{
    ASSERT_NE(agent->getTransform(), nullptr);
}