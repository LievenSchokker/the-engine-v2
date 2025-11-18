//
// Created by samle on 18/11/2025.
//

#include "GameObject/GameObject.h"
#include "Component/Component.h"
#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "../Component/TestComponentOne.h"
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