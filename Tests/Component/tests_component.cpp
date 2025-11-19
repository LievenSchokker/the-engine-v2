// //
// // Created by samle on 14/11/2025.
// //

#include "GameObject/GameObject.h"
#include "Component/Component.h"
#include "Component/ComponentManager.h"
#include "GameObject/ScenePlaceholder.h"
#include "../Component/TestComponents.h"

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

    TEST(ComponentTests, ComponentGetsDeletedOnGameObjectDestroy)
    {
        GameObject go;
        TestComponentOne* comp = go.addComponent<TestComponentOne>();

        /// Ensure component has been set up on the game object correctly.
        EXPECT_TRUE(go.hasComponent(comp));
        EXPECT_EQ(TestComponentBase::instanceCount, 1);

        EXPECT_FALSE(comp->destroyCalled);
        go.destroy();

        /// Normally the scene would call this after go.destroy was called at the end of the Update loop.
        /// But the scene destroys the GameObject right after, so we can't use that to test.
        /// Calling the function manually simulates what would happen when a GameObject would be deleted by the scene,
        /// but without deleting the GameObject from memeory itself (which happens right after scene would call this function)
        go.onSceneDestroy();

        /// Components should get their onDestroy() function called when their go.onSceneDestroy() function gets called.
        EXPECT_TRUE(comp->destroyCalled);

        /// Instance count gets decremented in the destructor, so should be 0 here (after deletion)
        EXPECT_EQ(TestComponentBase::instanceCount, 0);
    }


}
