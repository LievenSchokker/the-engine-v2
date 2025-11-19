//
// Created by samle on 18/11/2025.
//
#pragma once

#include "../Component/TestComponents.h"
#include "Behaviour/Behaviour.h"

namespace engine_tests
{
    class TestBehaviourBase : public Behaviour
    {
        public:
            TestBehaviourBase() = default;
            virtual ~TestBehaviourBase() = 0;

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


    class TestBehaviourOne : public TestBehaviourBase {};
    class TestBehaviourTwo : public TestBehaviourBase {};
    class TestBehaviourThree : public TestBehaviourBase {};
}
