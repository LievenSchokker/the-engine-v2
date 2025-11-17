// //
// // Created by samle on 14/11/2025.
// //

#include "../../../inc/engine/game_object/GameObject.h"
#include "../../../inc/engine/component/Component.h"
#include "../../../inc/engine/component/Transform.h"
#include "../../../inc/engine/component/ComponentManager.h"
#include "../component/TestComponentOne.h"
#include "../component/TestComponentTwo.h"

#include <gtest/gtest.h>

namespace engine_tests
{
    TEST(ComponentTests, AddComponentAddsToComponentManager)
    {
        GameObject go;
        ComponentManager* componentManager = go.getComponentManager();

        TestComponentOne* first = componentManager->addComponent<TestComponentOne>();
        EXPECT_NE(first, nullptr);
        EXPECT_TRUE(componentManager->hasComponent(first));

        TestComponentTwo* addedViaComponent = first->addComponent<TestComponentTwo>();
        EXPECT_NE(addedViaComponent, nullptr);
        EXPECT_TRUE(componentManager->hasComponent(addedViaComponent));

        TestComponentTwo* retrieved =  componentManager->getComponent<TestComponentTwo>();
        EXPECT_NE(retrieved, nullptr);
        EXPECT_EQ(addedViaComponent, retrieved);
    }


    TEST(ComponentTests, GetComponentReturnsFromComponentManager)
    {
        GameObject go;
        ComponentManager* componentManager = go.getComponentManager();
        TestComponentOne* added = componentManager->addComponent<TestComponentOne>();

        TestComponentOne* retrievedFromComponent = added->getComponent<TestComponentOne>();
        TestComponentOne* retrievedFromComponentManager = componentManager->getComponent<TestComponentOne>();

        EXPECT_NE(retrievedFromComponent, nullptr);
        EXPECT_EQ(retrievedFromComponent, retrievedFromComponentManager);
    }


    TEST(ComponentsTest, GameObjectSetsOnComponentManagerAdd)
    {
        GameObject go;
        ComponentManager* componentManager = go.getComponentManager();

        TestComponentOne* added = componentManager->addComponent<TestComponentOne>();

        EXPECT_EQ(added->getGameObject(), &go);
    }


    TEST(ComponentsTest, TransformReturnsGameObjectTransform)
    {
        GameObject go;
        ComponentManager* componentManager = go.getComponentManager();

        TestComponentOne* added = componentManager->addComponent<TestComponentOne>();

        EXPECT_EQ(added->getTransform(), go.getTransform());
    }


    TEST(ComponentTests, TryGetReturnsComponentManagerTryGet)
    {
        GameObject go;
        ComponentManager* componentManager = go.getComponentManager();

        TestComponentOne* added_1 = componentManager->addComponent<TestComponentOne>();
        TestComponentTwo* added_2 = componentManager->addComponent<TestComponentTwo>();

        TestComponentTwo* componentOut = nullptr;
        TestComponentTwo* managerOut = nullptr;

        bool fromAdded_1 = added_1->tryGetComponent<TestComponentTwo>(componentOut);
        bool fromManager = componentManager->tryGetComponent<TestComponentTwo>(managerOut);

        EXPECT_EQ(fromAdded_1, fromManager);
        EXPECT_EQ(componentOut, managerOut);
    }


    TEST(ComponentTests, SetGameObjectSetsPrivateFields)
    {
        GameObject go;
    }
}
