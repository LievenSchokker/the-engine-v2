//
// Created by samle on 14/12/2025.
//

#pragma once


#include "Behaviour/Behaviour.h"
#include "Component/Transform.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
class INavigationSurface;
class NavigationSystem;
class Agent;

class PathRenderer : public Behaviour, public RenderComponent
{
    public:
        explicit PathRenderer() : navigationSystem(nullptr), agent(nullptr), path({}), target(Vector2::zero()) {}
        ~PathRenderer() override = default;

        void onAwake() override;
        void setPath(const std::vector<Vector2>& newPath);
        void fillRenderQueue(IRenderQueueWriter& queue) const override;

        void setTarget(const Transform& t);
        void update(float deltaTime, GameWorld* world) override;

    private:
        void computeNewPath();

        NavigationSystem* navigationSystem = nullptr;
        INavigationSurface* navSurface = nullptr;
        Agent* agent = nullptr;

        std::vector<Vector2> path;
        Vector2 target;
        GameObject* player;
};
