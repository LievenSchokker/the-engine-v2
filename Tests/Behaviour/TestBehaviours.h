//
// Created by samle on 18/11/2025.
//
#pragma once

#include "Behaviour/Behaviour.h"

namespace engine_tests
{
    class TestBehaviourBase : public Behaviour
    {
        public:
            TestBehaviourBase() = default;
            virtual ~TestBehaviourBase() = 0;

            void onAwake() override;
            void onEnable() override;
            void onStart() override;
    		void update(float deltaTime, GameWorld* world) override;
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
