//
// Created by samle on 10/12/2025.
//

#pragma once

#include "Behaviour/Behaviour.h"


class PathRenderer;
class Agent;
class NavigationGrid;
class InputManager;
class NavigationSystem;

class NavigationTest : public Behaviour
{
    public:
        explicit NavigationTest() : input(nullptr),  target(nullptr), pathRenderer(nullptr), agent(nullptr) {};
        ~NavigationTest() override = default;

        void onAwake() override;
        void update(float deltaTime, GameWorld *world) override;

        void setTarget(const Transform& targetTransform);
        void setAgent(Agent& agent);

    private:
        InputManager* input;
        PathRenderer* pathRenderer;
        const Transform* target;
        Agent* agent;
};
