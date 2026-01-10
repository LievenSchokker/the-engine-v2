//
// Created by samle on 14/12/2025.
//

#pragma once


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
class PathRenderer : public RenderComponent
{
    public:
        explicit PathRenderer() :  pathRenderOptions({3.0f, Color::darkOrange()}) {}
        explicit PathRenderer(PathRenderOptions renderOptions) : pathRenderOptions(renderOptions) {}
        ~PathRenderer() override = default;

        PathRenderer& setOrderInLayer(int8_t order)
        {
            orderInLayer = order;
            return *this;
        }

        PathRenderer& setRenderEnabled(bool enabled)
        {
            renderEnabled = enabled;
            return *this;
        }

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
        PathRenderOptions pathRenderOptions;
        PathResult currentPath;
        int8_t orderInLayer = 0;
        bool renderEnabled = true;
};
