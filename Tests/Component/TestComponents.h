//
// Created by samle on 19/11/2025.
//


#pragma once


#include "Component/BaseComponentTypes/Component.h"


namespace engine_tests
{
    class TestComponentBase : public Component
    {
        public:
            TestComponentBase();
            ~TestComponentBase() override;

            void onDestroy() override;

            static int instanceCount;
    		static int destroyCount;
            bool destroyCalled = false;

    };

    class TestComponentOne : public TestComponentBase {};
    class TestComponentTwo : public TestComponentBase {};
    class TestComponentThree : public TestComponentBase {};
}