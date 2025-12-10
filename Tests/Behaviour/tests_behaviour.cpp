//
// Created by samle on 18/11/2025.
//
#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "TestBehaviours.h"
#include "../Component/TestComponents.h"

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
        TestBehaviourOne b;
        EXPECT_FALSE(b.getHasAwakened());
        b.awake();
        EXPECT_TRUE(b.getHasAwakened());
    }

    TEST(BehaviourTests, StartSetsHasStarted)
    {
        TestBehaviourOne b;
        EXPECT_FALSE(b.getHasStarted());
        b.start();
        EXPECT_TRUE(b.getHasStarted());
    }

    TEST(BehaviourTests, SetEnabledCallsHooks)
    {
        GameObject go;
        TestBehaviourOne* b = go.addComponent<TestBehaviourOne>();

        EXPECT_TRUE(b->getIsEnabled());

        b->setEnabled(false);
        EXPECT_FALSE(b->getIsEnabled());
        EXPECT_TRUE(b->disableCalled);

        b->setEnabled(true);
        EXPECT_TRUE(b->getIsEnabled());
        EXPECT_TRUE(b->enableCalled);
    }


    /// setEnabled(true) should return if gameObject is nullptr
    TEST(BehaviourTests, EnableBehaviourWithMissingGameObjectFails)
    {
        TestBehaviourOne b;

        EXPECT_TRUE(b.getIsEnabled());

        b.setEnabled(false);
        EXPECT_FALSE(b.getIsEnabled());
        EXPECT_TRUE(b.disableCalled);

        b.setEnabled(true);
        EXPECT_FALSE(b.getIsEnabled());
        EXPECT_FALSE(b.enableCalled);
    }

    TEST(BehaviourTests, EnableBehaviourWithDestroyedGameObjectFails)
    {
        GameObject go;
        TestBehaviourOne* b = go.addComponent<TestBehaviourOne>();

        EXPECT_TRUE(b->getIsEnabled());

        b->setEnabled(false);
        EXPECT_FALSE(b->getIsEnabled());
        EXPECT_TRUE(b->disableCalled);

        /// Destory the GO, which should prevent behaviour.setEnabled(true) from executing.
        go.destroy();
        b->setEnabled(true);
        EXPECT_FALSE(b->getIsEnabled());
        EXPECT_FALSE(b->enableCalled);
    }

    TEST(BehaviourTests, SetEnabledDoesNotCallHooksWhenNotChanged) {
        TestBehaviourOne b;
        b.enableCalled = false;
        b.disableCalled = false;

        b.setEnabled(true);
        EXPECT_FALSE(b.enableCalled);
        EXPECT_FALSE(b.disableCalled);
    }

    TEST(BehaviourTests, ActiveAndEnabled)
    {
        GameObject go;
        go.setActive(true);
        TestBehaviourOne* behaviour = go.addComponent<TestBehaviourOne>();

        behaviour->setEnabled(true);
        EXPECT_TRUE(behaviour->getIsActiveAndEnabled());

        behaviour->setEnabled(false);
        EXPECT_FALSE(behaviour->getIsActiveAndEnabled());

        go.setActive(false);
        behaviour->setEnabled(true);
        EXPECT_FALSE(behaviour->getIsActiveAndEnabled());
    }

    TEST(BehaviourTest, ConstructorSetsInitialStatesCorrect)
    {
        TestBehaviourOne b;

        EXPECT_TRUE(b.getIsEnabled());
        EXPECT_FALSE(b.getHasAwakened());
        EXPECT_FALSE(b.getHasStarted());
        EXPECT_FALSE(b.destroyCalled);
    }

    TEST(BehaviourTest, ComponentInheritedMethods)
    {
        /// setup.
        GameObject go;
        TestBehaviourOne* behaviour = go.addComponent<TestBehaviourOne>();

        /// 1) Inherited getGameObject function.
        EXPECT_EQ(behaviour->getGameObject(), &go);

        /// 2) Inherited getComponent function
        TestComponentOne* retrievedFromGo = go.getComponent<TestComponentOne>();
        TestComponentOne* retrievedFromBehaviour = behaviour->getComponent<TestComponentOne>();

        /// Checks if the inherited getComponent function called from behaviour returns the same component as the one returned from go.GetComponent
        EXPECT_EQ(retrievedFromGo, retrievedFromBehaviour);

        /// 3) Inherited addComponent function
        TestComponentThree* addedFromBehaviour = behaviour->addComponent<TestComponentThree>();

        /// Checks if the inherited addComponent function works as intended:
        /// The GO should now hold the by Behaviour added component.
        EXPECT_TRUE(go.hasComponent(addedFromBehaviour));

        /// 4) Inherited TryGetComponent function.
        go.addComponent<TestBehaviourTwo>();
        TestBehaviourTwo* outTryGetFromGo = nullptr;
        TestBehaviourTwo* outTryGetFromBehaviour = nullptr;

        bool tryGetFromGo = go.tryGetComponent<TestBehaviourTwo>(outTryGetFromGo);
        bool tryGetFromBehaviour = go.tryGetComponent<TestBehaviourTwo>(outTryGetFromBehaviour);

        EXPECT_EQ(tryGetFromGo, tryGetFromBehaviour);
        EXPECT_EQ(outTryGetFromGo, outTryGetFromBehaviour);
    }
}
