//
// Created by samle on 10/12/2025.
//

#pragma once

#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "AI/Navigation/PathResult.h"

class NavigationGrid;
class InputManager;
class NavigationSystem;

class NavigationTest : public Behaviour, public RenderComponent
{
    public:
        explicit NavigationTest() : input(nullptr), navSystem(nullptr), target(nullptr), grid(nullptr) {};
        ~NavigationTest() override = default;

        void onAwake() override;
        void update(float deltaTime, GameWorld *world) override;

        void fillRenderQueue(IRenderQueueWriter &queue) const override;

        void createPath();
        void showPath( );
        void setTarget(const Transform& targetTransform);

    private:
        InputManager* input;
        NavigationSystem* navSystem;
        const Transform* target;
        NavigationGrid* grid;

        PathResult pathResult;
        int currentWaypointIndex;
        bool showPathDebug;
};
