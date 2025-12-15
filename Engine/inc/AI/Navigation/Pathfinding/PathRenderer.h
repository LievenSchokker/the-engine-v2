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


struct PathRenderOptions
{
    float circleRadius = 3.0f;
    Color circleColor = Color::darkOrange();
};


class PathRenderer : public Behaviour, public RenderComponent
{
    public:
        explicit PathRenderer() : navigationSystem(nullptr), path({}), pathRenderOptions({3.0f, Color::darkOrange()}) {}
        explicit PathRenderer(PathRenderOptions renderOptions) : navigationSystem(nullptr), path({}),  pathRenderOptions(renderOptions) {}
        ~PathRenderer() override = default;

        void onAwake() override;
        void setPath(const std::vector<Vector2>& newPath);
        void fillRenderQueue(IRenderQueueWriter& queue) const override;

    private:
        NavigationSystem* navigationSystem = nullptr;
        PathRenderOptions pathRenderOptions;
        std::vector<Vector2> path;
};
