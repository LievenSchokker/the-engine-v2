//
// Created by samle on 14/12/2025.
//

#pragma once


#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "AI/Navigation/Pathfinding/PathResult.h"

class INavigationSurface;
class NavigationSystem;
class Agent;


/// @brief Rendering options belonging to the @c PathRenderer class
struct PathRenderOptions
{
    float circleRadius = 3.0f;
    Color validPathColor = Color::darkOrange();
    Color invalidPathColor = Color::darkPurple();
};


/**
 * @brief Render component that visualises a pathfinding path by drawing circles on their waypoints
 *
 * Uses the scene's NavigationSystem to access its INavigationSurface to be able to visualise the points on the path.
 */
class PathRenderer : public Behaviour, public RenderComponent
{
    public:
        explicit PathRenderer() : navigationSystem(nullptr), pathRenderOptions({3.0f, Color::darkOrange()}) {}
        explicit PathRenderer(PathRenderOptions renderOptions) : navigationSystem(nullptr),  pathRenderOptions(renderOptions) {}
        ~PathRenderer() override = default;

        /**
         * @brief onAwake() is used to retrieve the scene's navigation system.
         * If the system is not found, this behaviour silently gets disabled to prevent errors occuring
         */
        void onAwake() override;

        /**
         * @brief Visualise the path.
         * @param queue
         */
        void fillRenderQueue(IRenderQueueWriter& queue) const override;

        /**
         * @brief Sets the current path to visualise.
         * @param newPath
         */
        void setPath(const PathResult& newPath);

    private:
        NavigationSystem* navigationSystem = nullptr;

        PathRenderOptions pathRenderOptions;
        PathResult currentPath;
};
