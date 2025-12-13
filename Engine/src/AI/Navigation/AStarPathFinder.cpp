//
// Created by samle on 10/12/2025.
//


#include "../../../inc/AI/Navigation/Pathfinding/AStarPathFinder.h"
#include "../../../inc/AI/Navigation/Pathfinding/PathResult.h"

#include <queue>
#include <unordered_set>
#include <unordered_map>


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
    Vector2 goal;

    explicit CompareAStarNodes(const Vector2& goal) : goal(goal) {}

    bool operator()(const AStarNode& a, const AStarNode& b) const
    {
        int fA = a.fCost();
        int fB = b.fCost();

        if (fA == fB)
            return Vector2::distance(a.position, goal) > Vector2::distance(b.position, goal);

        return fA > fB;
    }
};


AStarPathFinder::AStarPathFinder()
{
    heuristicType = HeuristicType::EUCLIDIAN;
}


PathResult AStarPathFinder::findPath(const IPathfindingGraph& graph, Vector2 start, Vector2 end) const
{
    if (!graph.isValid(start) || !graph.isValid(end))
    {
        /// Return empty result if start or end is not valid
        return PathResult{{}};
    }
    CompareAStarNodes comparator(end);
    std::priority_queue<AStarNode, std::vector<AStarNode>, CompareAStarNodes> openSet(comparator);
    std::unordered_map<Vector2, AStarNode, Vector2Hash> traversalHistory;
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

        for (Vector2 neighbour : graph.getNeighbours(currentNode.position))
        {
            if (visited.contains(neighbour))
                continue;

            if (!graph.isValid(neighbour))
                continue;

            AStarNode neighbourNode;
            neighbourNode.position = neighbour;
            neighbourNode.parentPosition = currentNode.position;
            neighbourNode.gCost = currentNode.gCost + graph.getCost(currentNode.position, neighbourNode.position);
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
    switch (heuristicType)
    {
        case HeuristicType::MANHATTEN:
            return static_cast<int>(std::abs(from.x - to.x) + std::abs(from.y - to.y));
        case HeuristicType::EUCLIDIAN:
            return static_cast<int>(Vector2::distance(from, to));
        case HeuristicType::CHEBYSHEV:
            return static_cast<int>(std::max(std::abs(to.x - from.x), std::abs(to.y - from.y)));
        default:
            return static_cast<int>(Vector2::distance(from, to));
    }
}





