//
// Created by samle on 15/11/2025.
//

#include "GameObject/GameObject.h"
#include "Component/BaseComponentTypes/Component.h"
#include "Component/Transform.h"
#include "../Behaviour/TestBehaviours.h"
#include "TestComponents.h"

#include <gtest/gtest.h>


namespace engine_tests
{
    TEST(ComponentManagerTests, AddComponentAddsToComponents)
    {
        GameObject go{};
        TestComponentOne* testComponentOne = go.addComponent<TestComponentOne>();
        EXPECT_TRUE(go.hasComponent(testComponentOne));
    }


    TEST(ComponentManagerTests, AddComponentPreventsDuplicates)
    {
        GameObject go{};
        TestComponentOne* first = go.addComponent<TestComponentOne>();
        TestComponentOne* second = nullptr;

        EXPECT_NO_THROW(second = go.addComponent<TestComponentOne>());
        EXPECT_NE(first, nullptr);
        EXPECT_EQ(first, second);
        EXPECT_TRUE(go.hasComponent<TestComponentOne>());
        EXPECT_EQ(go.getComponentCount(), 1);
    }


    TEST(ComponentManagerTests, AddTransformComponentNotAllowed)
    {
        GameObject go{};
        Transform* transform = go.addComponent<Transform>();
        EXPECT_EQ(transform, nullptr);
    }


    TEST(ComponentManagerTests, GetTransformReturnsGameObjectTransform)
    {
        GameObject go{};
        Transform* transform = go.getComponent<Transform>();
        EXPECT_EQ(transform, go.getTransform());
    }


    TEST(ComponentManagerTests, GetComponentReturnsFromComponents)
    {
        GameObject go{};
        TestComponentOne* added = go.addComponent<TestComponentOne>();
        EXPECT_NE(added, nullptr);

        TestComponentOne* retrieved = go.getComponent<TestComponentOne>();
        EXPECT_NE(retrieved, nullptr);
        EXPECT_EQ(added, retrieved);
    }


    TEST(ComponentManagerTests, GetComponentNoErrorOnMissingComponent)
    {
        GameObject go{};
        TestComponentOne* comp =nullptr;

        EXPECT_NO_THROW(comp = go.getComponent<TestComponentOne>());
        EXPECT_EQ(comp, nullptr);
    }


    TEST(ComponentManagerTests, TryGetComponentReturnsCorrectBool)
    {
        GameObject go{};

        go.addComponent<TestComponentOne>();
        EXPECT_TRUE(go.hasComponent<TestComponentOne>());
        EXPECT_FALSE(go.hasComponent<TestComponentTwo>());

        TestComponentOne* outFirst = nullptr;
        TestComponentTwo* outSecond = nullptr;
        bool hasFirst = false;
        bool hasSecond = true;

        EXPECT_NO_THROW(hasFirst = go.tryGetComponent<TestComponentOne>(outFirst));
        EXPECT_NO_THROW(hasSecond = go.tryGetComponent<TestComponentTwo>(outSecond));

        EXPECT_TRUE(hasFirst);
        EXPECT_FALSE(hasSecond);
    }


    TEST(ComponentManagerTests, TryGetComponentSetsOutParameterCorrect)
    {
        GameObject go;

        TestComponentOne* added = go.addComponent<TestComponentOne>();
        EXPECT_TRUE(go.hasComponent<TestComponentOne>());

        TestComponentOne* outComponent = nullptr;
        bool tryGet = go.tryGetComponent<TestComponentOne>(outComponent);

        EXPECT_NE(outComponent, nullptr);
        EXPECT_EQ(added, outComponent);
    }


    TEST(ComponentManagerTests, RemoveComponentRemovesFromComponents)
    {
        GameObject go{};

        TestComponentOne* added = go.addComponent<TestComponentOne>();
        EXPECT_TRUE(go.hasComponent(added));

        go.removeComponent(added);
        EXPECT_FALSE(go.hasComponent(added));
    }


    TEST(ComponentManagerTests, RemoveComponentNoErrorOnMissingComponent)
    {
        GameObject go{};

        EXPECT_NO_THROW(go.removeComponent<TestComponentOne>());
    }

    TEST(ComponentManagerTests, StoresBehavioursInComponents)
    {
        GameObject go{};

        go.addComponent<TestComponentOne>();
        go.addComponent<TestBehaviourOne>();
        go.addComponent<TestBehaviourTwo>();
        go.addComponent<TestBehaviourThree>();

        EXPECT_EQ(go.getComponentCount(), 4);
        EXPECT_TRUE(go.hasComponent<TestBehaviourOne>());
        EXPECT_TRUE(go.hasComponent<TestBehaviourTwo>());
        EXPECT_TRUE(go.hasComponent<TestBehaviourThree>());
    }

    TEST(ComponentManagerTests, AddGetComponentWorksForBehaviours)
    {
        GameObject go{};

        TestBehaviourOne* addedBehaviour = go.addComponent<TestBehaviourOne>();

        EXPECT_TRUE(go.hasComponent<TestBehaviourOne>());

        TestBehaviourBase* retrievedBehaviour = go.getComponent<TestBehaviourOne>();

        EXPECT_NE(retrievedBehaviour, nullptr);
        EXPECT_EQ(addedBehaviour, retrievedBehaviour);
    }

    TEST(ComponentManagerTests, GetAllBehavioursReturnsOnlyBehaviours)
    {
        GameObject go{};

        TestComponentOne* addedComponent = go.addComponent<TestComponentOne>();
        TestBehaviourBase* addedBehaviour1 = go.addComponent<TestBehaviourOne>();
        TestBehaviourBase* addedBehaviour2 = go.addComponent<TestBehaviourTwo>();
        TestBehaviourBase* addedBehaviour3 = go.addComponent<TestBehaviourThree>();

        EXPECT_TRUE(go.hasComponent<TestBehaviourThree>());

        auto allBehaviours = go.getAllBehaviours();

        EXPECT_LT(allBehaviours.size(), go.getComponentCount());
        EXPECT_EQ(allBehaviours.size(), 3);
    }

    TEST(ComponentManagerTests, ComponentManagerDeletesAllComponentsOnGameObjectDestroy)
    {
        GameObject go{};

        go.addComponent<TestComponentOne>();
        go.addComponent<TestComponentTwo>();
        go.addComponent<TestComponentThree>();

        EXPECT_EQ(go.getComponentCount(), 3);
        EXPECT_EQ(go.getComponentCount(), 3);

        go.destroy();
        go.onSceneDestroy(); /// Normally scene would call this after go.destroy() was called, but then we dont have acces to the GO anymore, so simulate it instead.

        EXPECT_EQ(go.getComponentCount(), 0);
        EXPECT_EQ(go.getComponentCount(), 0);
    }
}
