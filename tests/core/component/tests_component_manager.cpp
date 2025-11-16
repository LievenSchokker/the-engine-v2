//
// Created by samle on 15/11/2025.
//

#include "../../../include/core/game_object/GameObject.h"
#include "../../../include/core/component/Component.h"
#include "../../../include/core/component/Transform.h"
#include "../../../include/core/component/ComponentManager.h"
#include "../component/TestComponentOne.h"
#include "../component/TestComponentTwo.h"

#include <gtest/gtest.h>


namespace engine_tests
{
    TEST(ComponentManagerTests, AddComponentAddsToComponents)
    {
        GameObject go;
        ComponentManager componentManager(&go);
        TestComponentOne* testComponentOne = componentManager.addComponent<TestComponentOne>();
        EXPECT_TRUE(componentManager.hasComponent(testComponentOne));
    }

    TEST(ComponentManagerTests, AddComponentPreventsDuplicates)
    {
        GameObject go;
        ComponentManager componentManager(&go);
        TestComponentOne* first = componentManager.addComponent<TestComponentOne>();
        TestComponentOne* second = nullptr;

        EXPECT_NO_THROW(second = componentManager.addComponent<TestComponentOne>());
        EXPECT_NE(first, nullptr);
        EXPECT_EQ(first, second);
        EXPECT_TRUE(componentManager.hasComponent<TestComponentOne>());
        EXPECT_EQ(componentManager.getComponentCount(), 1);
    }


    TEST(ComponentManagerTests, AddTransformComponentNotAllowed)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        Transform* transform = componentManager.addComponent<Transform>();
        EXPECT_EQ(transform, nullptr);
    }


    TEST(ComponentManagerTests, GetTransformReturnsGameObjectTransform)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        Transform* transform = componentManager.getComponent<Transform>();
        EXPECT_EQ(transform, go.getTransform());
    }

    TEST(ComponentManagerTests, GetComponentReturnsFromComponents)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        TestComponentOne* added = componentManager.addComponent<TestComponentOne>();
        EXPECT_NE(added, nullptr);

        TestComponentOne* retrieved = componentManager.getComponent<TestComponentOne>();
        EXPECT_NE(retrieved, nullptr);
        EXPECT_EQ(added, retrieved);
    }


    TEST(ComponentManagerTests, GetComponentNoErrorOnMissingComponent)
    {
        GameObject go;
        ComponentManager componentManager(&go);
        TestComponentOne* comp =nullptr;

        EXPECT_NO_THROW(comp = componentManager.getComponent<TestComponentOne>());
        EXPECT_EQ(comp, nullptr);
    }

    TEST(ComponentManagerTests, TryGetComponentReturnsCorrectBool)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        componentManager.addComponent<TestComponentOne>();
        EXPECT_TRUE(componentManager.hasComponent<TestComponentOne>());
        EXPECT_FALSE(componentManager.hasComponent<TestComponentTwo>());

        TestComponentOne* outFirst = nullptr;
        TestComponentTwo* outSecond = nullptr;
        bool hasFirst = false;
        bool hasSecond = true;

        EXPECT_NO_THROW(hasFirst = componentManager.tryGetComponent<TestComponentOne>(outFirst));
        EXPECT_NO_THROW(hasSecond = componentManager.tryGetComponent<TestComponentTwo>(outSecond));

        EXPECT_TRUE(hasFirst);
        EXPECT_FALSE(hasSecond);
    }


    TEST(ComponentManagerTests, TryGetComponentSetsOutParameterCorrect)
    {
        GameObject go;
        ComponentManager componentManager(&go);
        TestComponentOne* added = componentManager.addComponent<TestComponentOne>();
        EXPECT_TRUE(componentManager.hasComponent<TestComponentOne>());

        TestComponentOne* outComponent = nullptr;
        bool tryGet = componentManager.tryGetComponent<TestComponentOne>(outComponent);

        EXPECT_NE(outComponent, nullptr);
        EXPECT_EQ(added, outComponent);
    }


    TEST(ComponentManagerTests, RemoveComponentRemovesFromComponents)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        TestComponentOne* added = componentManager.addComponent<TestComponentOne>();
        EXPECT_TRUE(componentManager.hasComponent(added));

        componentManager.removeComponent(added);
        EXPECT_FALSE(componentManager.hasComponent(added));
    }

    TEST(ComponentManagerTests, RemoveComponentNoErrorOnMissingComponent)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        EXPECT_NO_THROW(componentManager.removeComponent<TestComponentOne>());
    }
}