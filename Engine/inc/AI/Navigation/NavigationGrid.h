//
// Created by samle on 08/12/2025.
//

#pragma once


#include "INavigationSurface.h"
#include "NavigationGridGraphAdapter.h"
#include "AI/Navigation/NavigationCell.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "AI/Navigation/IPathfindingGraph.h"
#include "Math/Vector2.h"

class NavigationGrid : public RenderComponent, INavigationSurface
{
    public:
        explicit NavigationGrid() : width(0), height(0), cellSize(1,1),
            graphAdapter(std::make_unique<NavigationGridGraphAdapter>(*this))
        {

        };

        ~NavigationGrid() override = default;

        void generateGrid(Vector2 gridDimensions, Vector2 cellSize);

        bool isWalkable(Vector2 cellPosition) const;
        void setWalkable(Vector2 cellPosition, bool value);

        int getCellWeight(Vector2 cellPosition) const;
        void setCellWeight(Vector2 cellPosition, float weight);

        Vector2 getCellSize() const;
        void setCellSize(Vector2 size);

        bool isInGrid(Vector2 cellPosition) const;

        Vector2 worldToCellPosition(Vector2 worldPos) const;
        Vector2 cellToWorldPosition(Vector2 cellPos) const;

        const std::vector<NavigationCell>& getCells() const;

        int getWidth() const;
        int getHeight() const;

        std::vector<Vector2> getNeighbours(Vector2 cellPos, bool diagonalNeighbours) const;
        NavigationCell& getNavigationCell(Vector2 positionInGrid);
        void fillRenderQueue(IRenderQueueWriter &queue) const override;

        const IPathfindingGraph* getPathfindingGraph() const override;
        void createSurface(Vector2 size, const std::vector<BoundingBox>& obstacles) override;

    private:
        int width;
        int height;
        Vector2 cellSize;
        std::vector<NavigationCell> cells;
        std::unique_ptr<IPathfindingGraph> graphAdapter;
};
