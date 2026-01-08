//
// Created by samle on 10/12/2025.
//

#pragma once

#include "Behaviour/Behaviour.h"


class PathRenderer;
class Agent;
class InputManager;
class GridComponent;

/// NOTE: This is a temporary class used to test Agent navigation.
/// #TODO THIS SHOULD BE REMOVED FROM THE ENGINE BEFORE DELIVERING
class NavigationTest : public Behaviour
{
    public:
        explicit NavigationTest()
            : input(nullptr),
              pathRenderer(nullptr),
              target(nullptr),
              agent(nullptr),
              gridComponent(nullptr),
              wasAtTarget(false)
        {
        };
        ~NavigationTest() override = default;

        void onAwake() override;
        void update(double deltaTime, const GameWorld& world) override;

        void setTarget(Transform& targetTransform);
        void setAgent(Agent& agent);
        void setGridComponent(GridComponent& grid);

    private:
        InputManager* input;
        PathRenderer* pathRenderer;
        Transform* target;
        Agent* agent;
        GridComponent* gridComponent;
        bool wasAtTarget;
};
