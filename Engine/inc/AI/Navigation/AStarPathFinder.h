//
// Created by samle on 10/12/2025.
//


#pragma once


#include "HeuristicType.h"
#include "AI/Navigation/IPathFinder.h"
#include "Math/Vector2.h"

class  NavigationGrid;
enum class HeuristicType;
struct PathResult;


struct AStarOptions
{
    bool useOrdinalDirections = true;
    HeuristicType heuristic = HeuristicType::EUCLIDIAN;
};


class AStarPathFinder final : public IPathFinder
{
    public:
        explicit AStarPathFinder() : astarOptions({}) {}
        ~AStarPathFinder() override = default;

        PathResult findPath(const IPathfindingGraph& graph, Vector2 start, Vector2 end) const override;
        int calculateHeuristic(Vector2 from, Vector2 to) const;

        void setAStarOptions(const AStarOptions& options);
        AStarOptions getAStarOptions() const;

    private:
        AStarOptions astarOptions;
};
