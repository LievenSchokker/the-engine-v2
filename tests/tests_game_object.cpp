//
// Created by samle on 12/11/2025.
//



#include <gtest/gtest.h>
#include "../include/core/game_object/GameObject.h"

namespace engine_tests
{

}
class TestComponent : public Component
{
    public:
        TestComponent() = default;
        ~TestComponent() override = default;
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
    go.addComponent<TestComponent>();
    TestComponent* comp = go.getComponent<TestComponent>();

    EXPECT_NE(comp, nullptr);
    EXPECT_EQ(comp, go.getComponentManager()->getComponent<TestComponent>());
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
    go.addComponent<TestComponent>();

    EXPECT_NE(go.getComponent<TestComponent>(), nullptr);
    go.removeComponent<TestComponent>();
    EXPECT_EQ(go.getComponentManager()->getComponent<TestComponent>(), nullptr);
}