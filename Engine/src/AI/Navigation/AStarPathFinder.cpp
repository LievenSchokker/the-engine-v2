//
// Created by samle on 10/12/2025.
//


#include "AI/Navigation/AStarPathFinder.h"

#include <filesystem>
#include <map>
#include <queue>
#include <unordered_set>
#include <unordered_map>

#include "AI/Navigation/CompassDirections.h"
#include "AI/Navigation/HeuristicType.h"
#include "AI/Navigation/PathResult.h"
#include "AI/Navigation/NavigationCell.h"

struct AStarNode
{
    int fCost() const { return gCost + hCost; };

    int gCost = 0;
    int hCost = 0;

    Vector2 position = Vector2::zero();
    Vector2 parentPosition = Vector2::zero();
};

struct CompareAStarNodes
{
    bool operator()(const AStarNode& a, const AStarNode& b) const
    {
        return a.fCost() > b.fCost();
    }
};


PathResult AStarPathFinder::findPath(const NavigationGrid& grid, Vector2 start, Vector2 end) const
{
    if (!grid.isValidCell(start) || !grid.isValidCell(end))
    {
        /// Return empty result if start or end is not valid
        return PathResult{{}};
    }

    std::priority_queue<AStarNode, std::vector<AStarNode>, CompareAStarNodes> openSet; /// This gets filled by the algorithm on the go, after adding the start node
    std::unordered_map<Vector2, AStarNode, Vector2Hash> traversalHistory; /// Vector2 is the position in the grid, AStarNode is the node on that position. Use its parent to get the 'next'  path element.
    std::unordered_set<Vector2, Vector2Hash> visited;


    AStarNode startNode;
    startNode.position = start;
    startNode.gCost = 0;
    startNode.hCost = calculateHeuristic(start, end);

    openSet.push(startNode);
    bool pathFound = false;

    while (!openSet.empty())
    {
        /// Get node with most prio.
        AStarNode currentNode = openSet.top();
        openSet.pop();

        /// Already visited, skip.
        if (visited.contains(currentNode.position))
            continue;

        visited.emplace(currentNode.position);

        if (currentNode.position == end)
        {
            pathFound = true;
            break;
        }

        for (Vector2 neighbour : grid.getNeighbours8D(currentNode.position))
        {
            if (visited.contains(neighbour))
                continue;

            if (!grid.isWalkable(neighbour))
                continue;

            AStarNode neighbourNode;
            neighbourNode.position = neighbour;
            neighbourNode.parentPosition = currentNode.position;
            neighbourNode.gCost = currentNode.gCost + grid.getCellWeight(neighbour);
            neighbourNode.hCost = calculateHeuristic(neighbour, end);

            auto it = traversalHistory.find(neighbourNode.position);

            if (it == traversalHistory.end() || neighbourNode.gCost < it->second.gCost)
            {
                traversalHistory[neighbour] = neighbourNode;
                openSet.push(neighbourNode);
            }
        }
    }

    std::vector<Vector2> reconstructedPath;

    if (pathFound)
    {
        Vector2 current = end;

        while (current != start)
        {
            reconstructedPath.push_back(current);
            current = traversalHistory[current].parentPosition;
        }

        std::reverse(reconstructedPath.begin(), reconstructedPath.end());
    }

    return PathResult{reconstructedPath};
}

int AStarPathFinder::calculateHeuristic(Vector2 from, Vector2 to) const
{
    switch (heuristic)
    {
        case HeuristicType::MANHATTEN:
            break;
        case HeuristicType::EUCLIDIAN:
            break;
        case HeuristicType::CHEBYSHEV:
            break;
        default:
            break;
    }

    return 0;
}


HeuristicType AStarPathFinder::getHeuristicType() const
{
    return heuristic;
}

void AStarPathFinder::setHeuristicType(HeuristicType heuristicType)
{
    heuristic = heuristicType;
}




