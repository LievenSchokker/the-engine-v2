//
// Created by samle on 18/11/2025.
//
#pragma once
#include "Behaviour/Behaviour.h"

namespace engine_tests
{
    class TestBehaviour : public Behaviour
    {
        public:
            TestBehaviour() = default;
            virtual ~TestBehaviour() = 0;

            void awake() override;
            void onEnable() override;
            void start() override;
            void update() override;
            void onDisable() override;
            void onDestroy() override;

            bool enableCalled = false;
            bool disableCalled = false;
            bool awakeCalled = false;
            bool startCalled = false;
            bool updateCalled = false;
            bool destroyCalled = false;
    };

    class TestBehaviourOne : public TestBehaviour
    {
        public:
            TestBehaviourOne() = default;
            ~TestBehaviourOne() override = default;
    };

    class TestBehaviourTwo : public TestBehaviour
    {
        public:
            TestBehaviourTwo() = default;
            ~TestBehaviourTwo() override = default;
    };

    class TestBehaviourThree : public TestBehaviour
    {
        public:
            TestBehaviourThree() = default;
            ~TestBehaviourThree() override = default;
    };

}
