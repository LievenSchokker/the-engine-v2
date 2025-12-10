//
// Created by samle on 10/12/2025.
//


#pragma once


#include "AI/Navigation/IPathFinder.h"
#include "Math/Vector2.h"

class  NavigationGrid;
enum class HeuristicType;
struct PathResult;


class AStarPathFinder final : public IPathFinder
{
    public:
        explicit AStarPathFinder(HeuristicType heuristicType) : heuristic(heuristicType) {}
        ~AStarPathFinder() override = default;

        PathResult findPath(const NavigationGrid& grid, Vector2 start, Vector2 end) const override;
        int calculateHeuristic(Vector2 from, Vector2 to) const;

        HeuristicType getHeuristicType() const;
        void setHeuristicType(HeuristicType heuristic);

    private:
        HeuristicType heuristic;
};
