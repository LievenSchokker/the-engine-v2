//
// Created by samle on 12/11/2025.
//


#include <gtest/gtest.h>
#include "../include/core/game_object/GameObject.h"

namespace engine_tests
{
    class TestComponentOne : public Component
    {
    public:
        TestComponentOne() = default;
        ~TestComponentOne() override = default;
    };

    class TestComponentTwo : public Component
    {
    public:
        TestComponentTwo() = default;
        ~TestComponentTwo() override = default;
    };

    class TestComponentThree : public Component
    {
    public:
        TestComponentThree() = default;
        ~TestComponentThree() override = default;
    };

    TEST(GameObjectTests, HasTransformAfterConstruction)
    {
        GameObject go;
        EXPECT_NE(go.getTransform(), nullptr);
    }

    TEST(GameObjectTests, HasComponentManagerAfterConstruction)
    {
        GameObject go;
        EXPECT_NE(go.getComponentManager(), nullptr);
    }

    TEST(GameObjectTests, AddComponentAddsToComponentManager)
    {
        GameObject go;
        TestComponentOne* empty = go.getComponentManager()->getComponent<TestComponentOne>();
        EXPECT_EQ(empty, nullptr);

        go.addComponent<TestComponentOne>();
        TestComponentOne* goComp = go.getComponent<TestComponentOne>();
        TestComponentOne* managerComp = go.getComponentManager()->getComponent<TestComponentOne>();

        EXPECT_EQ(goComp, managerComp);
    }

    TEST(GameObjectTests, AddTransformDirectlyIsNotAllowed)
    {
        GameObject go;
        go.addComponent<Transform>();
        EXPECT_NE(go.getTransform(), nullptr);
        EXPECT_EQ(go.getComponentManager()->getComponent<Transform>(), nullptr);
    }


    TEST(GameObjectTests, GameObjectRemoveComponentFunctionRemovesViaComponentManager)
    {
        GameObject go;
        go.addComponent<TestComponentOne>();
        EXPECT_NE(go.getComponent<TestComponentOne>(), nullptr);
        go.removeComponent<TestComponentOne>();
        EXPECT_EQ(go.getComponentManager()->getComponent<TestComponentOne>(), nullptr);
    }

    TEST(GameObjectTests, ComponentManagerRemovesAllComponents)
    {
        GameObject go;
        go.addComponent<TestComponentOne>();
        go.addComponent<TestComponentTwo>();
        go.addComponent<TestComponentThree>();

        bool componentCountBiggerThanZero1 = go.getComponentManager()->components.size() > 0;
        EXPECT_EQ(componentCountBiggerThanZero1, true);
        go.getComponentManager()->removeAllComponents();
        bool componentCountBiggerThanZero2 = go.getComponentManager()->components.size() > 0;

        EXPECT_EQ(componentCountBiggerThanZero2, false);
    }

    TEST(GameObjectTests, GetComponentCount)
    {
        GameObject go;
        go.addComponent<TestComponentOne>();
        go.addComponent<TestComponentTwo>();
        go.addComponent<TestComponentThree>();

        EXPECT_EQ(go.getComponentCount(), 3);
    }
}
