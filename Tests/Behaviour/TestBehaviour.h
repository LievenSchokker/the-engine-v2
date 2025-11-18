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
            ~TestBehaviour() override = default;

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
}
