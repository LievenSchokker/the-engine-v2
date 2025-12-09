//
// Created by samle on 15/11/2025.
//

#include "GameObject/GameObject.h"
#include "../../Engine/inc/Component/BaseComponentTypes/Component.h"
#include "Component/Transform.h"
#include "Component/ComponentManager.h"
#include "../Behaviour/TestBehaviours.h"
#include "../Component/TestComponents.h"

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

    TEST(ComponentManagerTests, StoresBehavioursInComponents)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        componentManager.addComponent<TestComponentOne>();
        componentManager.addComponent<TestBehaviourOne>();
        componentManager.addComponent<TestBehaviourTwo>();
        componentManager.addComponent<TestBehaviourThree>();

        EXPECT_EQ(componentManager.getComponentCount(), 4);
        EXPECT_TRUE(componentManager.hasComponent<TestBehaviourOne>());
        EXPECT_TRUE(componentManager.hasComponent<TestBehaviourTwo>());
        EXPECT_TRUE(componentManager.hasComponent<TestBehaviourThree>());
    }

    TEST(ComponentManagerTests, AddGetComponentWorksForBehaviours)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        TestBehaviourOne* addedBehaviour = componentManager.addComponent<TestBehaviourOne>();

        EXPECT_TRUE(componentManager.hasComponent<TestBehaviourOne>());

        TestBehaviourBase* retrievedBehaviour = componentManager.getComponent<TestBehaviourOne>();

        EXPECT_NE(retrievedBehaviour, nullptr);
        EXPECT_EQ(addedBehaviour, retrievedBehaviour);
    }

    TEST(ComponentManagerTests, GetAllBehavioursReturnsOnlyBehaviours)
    {
        GameObject go;
        ComponentManager componentManager(&go);

        TestComponentOne* addedComponent = componentManager.addComponent<TestComponentOne>();

        TestBehaviourBase* addedBehaviour1 = componentManager.addComponent<TestBehaviourOne>();
        TestBehaviourBase* addedBehaviour2 = componentManager.addComponent<TestBehaviourTwo>();
        TestBehaviourBase* addedBehaviour3 = componentManager.addComponent<TestBehaviourThree>();

        EXPECT_TRUE(componentManager.hasComponent<TestBehaviourThree>());

        auto allBehaviours = componentManager.getAllBehaviours();

        EXPECT_LT(allBehaviours.size(), componentManager.getComponentCount());
        EXPECT_EQ(allBehaviours.size(), 3);
    }

    TEST(ComponentManagerTests, ComponentManagerDeletesAllComponentsOnGameObjectDestroy)
    {
        GameObject go;
        ComponentManager* componentManager = go.getComponentManager();

        go.addComponent<TestComponentOne>();
        go.addComponent<TestComponentTwo>();
        go.addComponent<TestComponentThree>();

        EXPECT_EQ(componentManager->getComponentCount(), 3);
        EXPECT_EQ(go.getComponentCount(), 3);

        go.destroy();
        go.onSceneDestroy(); /// Normally scene would call this after go.destroy() was called, but then we dont have acces to the GO anymore, so simulate it instead.

        EXPECT_EQ(componentManager->getComponentCount(), 0);
        EXPECT_EQ(go.getComponentCount(), 0);
    }
}
