//
// Created by samle on 10/12/2025.
//

#pragma once

#include "Behaviour/Behaviour.h"

#include <string>


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
              wasAtTarget(false),
              agentConfigured(false),
              agentName(),
              targetName(),
              gridName(),
              agentMaxSpeed(150.0f),
              agentTurnRate(180.0f),
              agentArriveDistance(4.0f),
              followPathWeight(150.0f),
              followPathRadius(4.0f)
        {
        };
        ~NavigationTest() override = default;

        void onAwake() override;
        void update(double deltaTime, const GameWorld& world) override;

        void setTarget(Transform& targetTransform);
        void setAgent(Agent& agent);
        void setGridComponent(GridComponent& grid);
        void setAgentConfig(float maxSpeed,
                            float turnRate,
                            float arrivingDistance,
                            float followPathWeight,
                            float followPathRadius);

    private:
        void resolveReferences();
        void configureAgentIfNeeded();

        InputManager* input;
        PathRenderer* pathRenderer;
        Transform* target;
        Agent* agent;
        GridComponent* gridComponent;
        bool wasAtTarget;
        bool agentConfigured;
        std::string agentName;
        std::string targetName;
        std::string gridName;
        float agentMaxSpeed;
        float agentTurnRate;
        float agentArriveDistance;
        float followPathWeight;
        float followPathRadius;
};
