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
            TestComponentBase() = default;
            ~TestComponentBase() = 0 {}
    };



    class TestComponentOne : public TestComponentBase
    {
        public:
            TestComponentOne() = default;
            ~TestComponentOne() = default;
    };



    class TestComponentTwo : public TestComponentBase
    {
        public:
            TestComponentTwo() = default;
            ~TestComponentTwo() = default;
    };



    class TestComponentThree : public TestComponentBase
    {
        public:
            TestComponentThree() = default;
            ~TestComponentThree() = default;
    };
}