//
// Created by samle on 12/11/2025.
//


#include "../../../Engine/inc/GameObject/GameObject.h"
#include "../../../Engine/inc/component/Component.h"
#include "../component/TestComponentOne.h"
#include "../component/TestComponentTwo.h"
#include "../component/TestComponentThree.h"

#include <gtest/gtest.h>


namespace engine_tests
{
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

    TEST(GameObjectTests, TAddComponentAddsToComponentManager)
    {
        GameObject go;
        TestComponentOne* comp = go.addComponent<TestComponentOne>();
        bool exists = go.getComponentManager()->hasComponent(comp);
        EXPECT_TRUE(exists);
    }


    TEST(GameObjectTests, GetComponentGetFromComponentManager)
    {
        GameObject go;

        auto& createdComp = *go.addComponent<TestComponentOne>();

        auto* fromGO = go.getComponent<TestComponentOne>();
        ASSERT_NE(fromGO, nullptr);
        EXPECT_EQ(fromGO, &createdComp);

        auto* fromManager = go.getComponentManager()->getComponent<TestComponentOne>();
        ASSERT_NE(fromManager, nullptr);
        EXPECT_EQ(fromManager, &createdComp);
    }

    TEST(GameObjectTests, TRemoveComponentRemovesFromComponentManager)
    {
        GameObject go;
        TestComponentOne* comp = go.addComponent<TestComponentOne>();
        ASSERT_NE(go.getComponentManager()->getComponent<TestComponentOne>(), nullptr);

        go.removeComponent<TestComponentOne>();
        TestComponentOne* managerComp = go.getComponentManager()->getComponent<TestComponentOne>();

        EXPECT_EQ(managerComp, nullptr);
    }

    TEST(GameObjectTests, RemoveComponentRemovesFromComponentManager)
    {
        GameObject go;
        TestComponentOne* comp = go.addComponent<TestComponentOne>();
        ASSERT_EQ(comp, go.getComponentManager()->getComponent<TestComponentOne>());

        go.removeComponent(comp);
        TestComponentOne* managerComp = go.getComponentManager()->getComponent<TestComponentOne>();

        EXPECT_EQ(managerComp, nullptr);
    }

    TEST(GameObjectTests, GetComponentCount)
    {
        GameObject go;
        go.addComponent<TestComponentOne>();
        go.addComponent<TestComponentTwo>();
        go.addComponent<TestComponentThree>();

        EXPECT_EQ(go.getComponentCount(), 3);
    }

    TEST(GameObjectTests, TryGetComponentSetsOutParam)
    {
        GameObject go;
        TestComponentOne* addedComp = go.addComponent<TestComponentOne>();

        TestComponentOne* comp = nullptr;
        bool hasComponent = go.tryGetComponent<TestComponentOne>(comp);
        ASSERT_TRUE(hasComponent);
        ASSERT_NE(comp, nullptr);

        EXPECT_EQ(comp, addedComp);
    }

    TEST(GameObjectTests, GetOrAddFunctionAddsMissingComponent)
    {
        GameObject go;
        ASSERT_EQ(go.getComponent<TestComponentOne>(), nullptr);

        TestComponentOne* comp = go.getOrAddComponent<TestComponentOne>();
        EXPECT_NE(comp, nullptr);

        TestComponentOne* retrieved = go.getComponent<TestComponentOne>();
        EXPECT_EQ(retrieved, comp);
    }

    TEST(GameObjectTests, GetOrAddComponentReturnsExistingComponent)
    {
        GameObject go;

        TestComponentOne* added = go.addComponent<TestComponentOne>();

        TestComponentOne* comp = go.getOrAddComponent<TestComponentOne>();

        EXPECT_NE(comp, nullptr);
        EXPECT_EQ(comp, added);

        TestComponentOne* retrieved = go.getComponent<TestComponentOne>();
        EXPECT_EQ(retrieved, added);
    }

    TEST(GameObjectTests, GetSetName)
    {
        GameObject go;

        // Note: Default name of a game object == GameObject (set in constructor).
        EXPECT_EQ(go.getName(), "GameObject");

        go.setName("Player");
        EXPECT_EQ(go.getName(), "Player");
    }

    TEST(GameObjectTests, GetSetCompareTag)
    {
        GameObject go;

        // Note: Default tag == "" (empty), set via construcotr.
        EXPECT_EQ(go.getTag(), "");

        go.setTag("Enemy");
        EXPECT_EQ(go.getTag(), "Enemy");

        EXPECT_TRUE(go.compareTag("Enemy"));
        EXPECT_FALSE(go.compareTag("Player"));
    }

    TEST(GameObjectTests, GetSetLayer)
    {
        GameObject go;

        // Note: Default layer == 0, set via constructor.
        EXPECT_EQ(go.getLayer(), 0);

        go.setLayer(5);
        EXPECT_EQ(go.getLayer(), 5);
    }

    TEST(GameObjectTests, GetSetIsStatic)
    {
        GameObject go;
        EXPECT_EQ(go.getIsStatic(), false);

        go.setIsStatic(true);
        EXPECT_EQ(go.getIsStatic(), true);
    }
}
