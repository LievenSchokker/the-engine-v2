//
// Created by samle on 18/11/2025.
//

#include "../../../Engine/inc/GameObject/GameObject.h"
#include "../../../Engine/inc/component/Component.h"
#include "../../../Engine/inc/behaviour/Behaviour.h"
#include "../../../Engine/inc/component/ComponentManager.h"
#include "../component/TestComponentOne.h"
#include "../component/TestComponentTwo.h"

#include <gtest/gtest.h>

namespace engine_tests
{
    TEST(BehaviourTests, PreventInstantiateBehaviourDirectly)
    {
        static_assert(std::is_abstract<Behaviour>::value,
                      "Behaviour must be an abstract class and cannot be instantiated directly.");

        EXPECT_TRUE(std::is_abstract<Behaviour>::value);
    }



}