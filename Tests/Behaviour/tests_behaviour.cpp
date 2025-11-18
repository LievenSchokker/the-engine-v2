//
// Created by samle on 18/11/2025.
//
#include "GameObject/GameObject.h"
#include "Component/Component.h"
#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "../Component/TestComponentOne.h"
#include "../component/TestComponentTwo.h"
#include "TestBehaviour.h"
#include <gtest/gtest.h>

namespace engine_tests
{
    TEST(BehaviourTests, PreventInstantiateBehaviourDirectly)
    {
        static_assert(std::is_abstract<Behaviour>::value,
                      "Behaviour must be an abstract class and cannot be instantiated directly.");
        EXPECT_TRUE(std::is_abstract<Behaviour>::value);
    }

    TEST(BehaviourTests, AwakeSetsHasAwakened)
    {
        TestBehaviour b;
        EXPECT_FALSE(b.getHasAwakened());
        b.awake();
        EXPECT_TRUE(b.getHasAwakened());
    }

    TEST(BehaviourTests, StartSetsHasStarted)
    {
        TestBehaviour b;
        EXPECT_FALSE(b.getHasStarted());
        b.start();
        EXPECT_TRUE(b.getHasStarted());
    }

    TEST(BehaviourTests, SetEnabledCallsHooks) {
        TestBehaviour b;

        EXPECT_TRUE(b.getIsEnabled());

        b.setEnabled(false);
        EXPECT_FALSE(b.getIsEnabled());
        EXPECT_TRUE(b.disableCalled);

        b.setEnabled(true);
        EXPECT_TRUE(b.getIsEnabled());
        EXPECT_TRUE(b.enableCalled);
    }

    TEST(BehaviourTests, SetEnabledDoesNotCallHooksWhenNotChanged) {
        TestBehaviour b;
        b.enableCalled = false;
        b.disableCalled = false;

        b.setEnabled(true);
        EXPECT_FALSE(b.enableCalled);
        EXPECT_FALSE(b.disableCalled);
    }

    TEST(BehaviourTests, ActiveAndEnabled) {
        GameObject go;
        go.setIsActive(true);
        TestBehaviour* behaviour = go.addComponent<TestBehaviour>();

        behaviour->setEnabled(true);
        EXPECT_TRUE(behaviour->getIsActiveAndEnabled());

        behaviour->setEnabled(false);
        EXPECT_FALSE(behaviour->getIsActiveAndEnabled());

        go.setIsActive(false);
        behaviour->setEnabled(true);
        EXPECT_FALSE(behaviour->getIsActiveAndEnabled());
    }
}
