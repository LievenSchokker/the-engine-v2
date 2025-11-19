//
// Created by samle on 19/11/2025.
//


#pragma once


#include "Component/Component.h"


namespace engine_tests
{
    class TestComponentBase : public Component
    {
        public:
            TestComponentBase();
            ~TestComponentBase() override;

            void onDestroy() override;

            static int instanceCount;
            bool destroyCalled = false;
    };

    class TestComponentOne : public TestComponentBase {};
    class TestComponentTwo : public TestComponentBase {};
    class TestComponentThree : public TestComponentBase {};
}