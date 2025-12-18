//
// Created by samle on 08/12/2025.
//

#pragma once


/**
 * @brief Represents a cell on the grid, used by @c NavigationGrid
 *
 * Defines a cell's weight and walkability, usefull for pathfinding
 */
struct GridCell
{
    bool walkable = true;
    int weight = 0;
};
