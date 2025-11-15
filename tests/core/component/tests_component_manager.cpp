//
// Created by samle on 15/11/2025.
//

#include "../../../include/core/game_object/GameObject.h"
#include "../../../include/core/component/Component.h"
#include "../../../include/core/component/ComponentManager.h"
#include "../component/TestComponentOne.h"
#include "../component/TestComponentTwo.h"
#include "../component/TestComponentThree.h"

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

    // TEST(ComponentManagerTests, AddComponentPreventsDuplicates)
    // {
    //
    // }
    //
    // TEST(ComponentManagerTests, GetComponentReturnsFromComponents)
    // {
    //
    // }
    //
    // TEST(ComponentManagerTests, GetComponentNoErrorOnMissingComponent)
    // {
    //
    // }
    //
    // TEST(ComponentManagerTests, TryGetComponentReturnsCorrectBool)
    // {
    //
    // }
    //
    //
    // TEST(ComponentManagerTests, TryGetComponentSetsOutParameterCorrect)
    // {
    //
    // }
    //
    //
    // TEST(ComponentMangagerTests, RemoveComponentRemovesFromComponents)
    // {
    //
    // }
    //
    // TEST(ComponentMangagerTests, RemoveComponentNoErrorOnMissingComponent)
    // {
    //
    // }
}