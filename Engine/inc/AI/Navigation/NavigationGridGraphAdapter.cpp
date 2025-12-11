//
// Created by samle on 11/12/2025.
//

#include "NavigationGridGraphAdapter.h"
#include "AI/Navigation/NavigationGrid.h"

float NavigationGridGraphAdapter::getCost(const Vector2 from, const Vector2 to) const
{
    Vector2 delta = to - from;

    bool isDiagonal = (std::abs(delta.x) > 0 && std::abs(delta.y) > 0);

    float movementCost = isDiagonal ? std::sqrt(2) : 1.0f;
    float cellCost = static_cast<float>(grid.getCellWeight(to));

    return movementCost + cellCost;
}


std::vector<Vector2> NavigationGridGraphAdapter::getNeighbours(const Vector2 node) const
{
    bool includeOrdinals = true; /// Replace with a valid solution to pass options or somehting
    return grid.getNeighbours(node, includeOrdinals);
}

bool NavigationGridGraphAdapter::isValid(const Vector2 node) const
{
    return (grid.isInGrid(node) && grid.isWalkable(node));
}

