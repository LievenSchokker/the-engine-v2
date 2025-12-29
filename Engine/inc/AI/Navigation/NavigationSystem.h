//
// Created by samle on 08/12/2025.
//

#pragma once


#include "AI/Navigation/INavigationSurface.h"

class IPathFinder;
class GameObject;
class NavigationObstacle;
class NavigationGrid;
struct BoundingBox;
struct PathResult;
struct Vector2;

#include <vector>
#include <memory>


/**
 * @brief System for navigation within the engine
 *
 * The system uses a concrete implementation of INavigationSurface and bakes it, making it usable for path finding
 *
 * The system uses its INavigationSurface to get compute a path between given points when requested, passing the pathfinder for it.
 */
class NavigationSystem
{
    public:
        explicit NavigationSystem(std::unique_ptr<INavigationSurface> navSurface) : navigationSurface(std::move(navSurface)) {}
        ~NavigationSystem() = default;

        /**
         * @brief Bakes this systems INavigationSurface by providing it's NavigationObstacles
         *
         * After baking, the surface can be used for pathfinding purposes
         * @param obstacles Generic vector of BoundingBoxes to use during baking.
         */
        void bake(const std::vector<BoundingBox>& obstacles) const;

        /**
         * @brief Uses this system's navigation surface to compute a path between two points, using a concrete path finder to get a PathResult
         * @param pathFinder Concrete pathfinder to use
         * @param start Start point of the path
         * @param end Target end point of the path
         * @return A pathResult, containing information about the computed path
         */
        PathResult computePath(const IPathFinder& pathFinder, Vector2 start, Vector2 end) const;

        /**
         * Sets this system's INavigationSurface
         *
         * Note: Does not automatically bake the surface, manually call the @c bake() method to bake the new surface
         * @param navSurface new surface to use
         */
        void setNavigationSurface(std::unique_ptr<INavigationSurface> navSurface);

        /**
         * Retrieves a raw pointer to this systems navigation surface.
         * @return
         */
        INavigationSurface* getNavigationSurface() const;

    private:
        std::unique_ptr<INavigationSurface> navigationSurface = nullptr;
};
