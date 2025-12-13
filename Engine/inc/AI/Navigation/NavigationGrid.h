//
// Created by samle on 08/12/2025.
//

#pragma once


#include "AI/Navigation/GridCell.h"
#include "INavigationSurface.h"
#include "Math/Vector2.h"
#include "NavigationGridGraphAdapter.h"
#include "Pathfinding/IPathfindingGraph.h"

#include <memory>


/**
 * @brief Concrete implementation of a INavigationSurface, representing a basic 2D grid of @c GridCell
 *
 * This class only stores the data needed to represent a grid, stored in a flattened std::vector instead of a 2D array.
 */
class NavigationGrid : public INavigationSurface
{
    public:
        explicit NavigationGrid(int gridWidth, int gridHeight, Vector2 cellSize) : width(0), height(0), cellSize(1,1),
            graphAdapter(std::make_unique<NavigationGridGraphAdapter>(*this))
        {

        };

        ~NavigationGrid() override = default;

        /// Interface implementation:
        const IPathfindingGraph* getPathfindingGraph() const override;
        void bakeSurface(const std::vector<BoundingBox>& obstacles)  override;
        Vector2 toWorldPoint(Vector2 surfacePoint) const override;
        Vector2 toSurfacePoint(Vector2 worldPoint) const override;
        bool isPointOnSurface(Vector2 surfacePoint) const override;

        /**
         * @brief creates a new grid by using the dimensions for its size, filling the @c cells with Vector2's
         * @param gridDimensions Widht and Height of the grid, represented in a Vector2. Gets rounded down to integer values
         * @param cellSize Size each cell in the grid should be.
         */
        void generateGrid(Vector2 gridDimensions, Vector2 cellSize);

        /**
        * @brief Converts a given world position to a position in the grid
        * @param worldPos The position to convert
        * @return The converted position
        */
        Vector2 worldToCellPosition(Vector2 worldPos) const;

        /**
         * @brief Converts a given position from the grid to world space
         * @param cellPos the position of the cell to convert
         * @return The position in world space
         */
        Vector2 cellToWorldPosition(Vector2 cellPos) const;

        /**
         * @brief Retrieves the @c GridCell existing on the provided position in the grid
         * @param positionInGrid position to retrieve the cell from
         * @return a Reference to the retrieved GridCell
         */
        GridCell& getCellAt(Vector2 positionInGrid);

        /**
         * @brief Retrieves the position of a given cell in the grid
         * @param cell the position to return of
         * @return The position in the grid. use @c cellToWorldPosition() to convert to world space coordinates.
         */
        Vector2 getCellPosition(const GridCell& cell) const;

        /**
         * @brief Checks whether a cell at the provided position exists within the grid
         * @param cellPosition Cell to look for in the grid
         * @return true if the provided position is in the grid, false otherwise
         */
        bool isInGrid(Vector2 cellPosition) const;

        /**
         * @brief Retrieves a reference to all the GridCells in this grid
         * @return a const reference vector of GridCells
         */
        const std::vector<GridCell>& getCells() const;

        /**
         * Retrieves every neighbour cell from the provided cell
         *
         * Can be used to get neighbours in 4 (NESW) or 8 (NESW + diagonals) directions
         * @param origin The position of the cell to look for neighbours from
         * @param diagonalNeighbours Should diagonal neighbours be included?
         * @return a vector<Vector2> containing each neighbour of the origin cell
         */
        std::vector<Vector2> getNeighbours(Vector2 origin, bool diagonalNeighbours) const;

        /**
         * @brief Checks whether the given cell in the grid isWalkable
         * @param cellPosition cell in the grid to check
         * @return true if walkable, false otherwise
         */
        bool isWalkable(Vector2 cellPosition) const;

        /**
         * Sets the isWalkable member of an @c GridCell to the provided value
         * @param cellPosition cell in the grid to set
         * @param value the new value of cell's isWalkable
         */
        void setWalkable(Vector2 cellPosition, bool value);

        /**
         * Retrieves teh weight member of GridCell at the given position
         * @param cellPosition
         * @return the weight
         */
        int getCellWeight(Vector2 cellPosition) const;

        /**
         * @brief Sets the weight member of the GridCell at the provided position in the grid
         * @param cellPosition cell in the grid to set
         * @param weight the new weight of the cell
         */
        void setCellWeight(Vector2 cellPosition, float weight);

        /**
         * @brief Retrieves the size of cells used for this grid
         * @return A vector2 containing the Widht and Height
         */
        Vector2 getCellSize() const;

        /**
         * @brief Sets the size of cells in the grid
         * @param size the desired size
         */
        void setCellSize(Vector2 size);

        /**
         * Retrieves the width of this grid
         * @return width
         */
        int getWidth() const;

        /**
         * @brief Retrieves the height of this grid
         * @return height
         */
        int getHeight() const;

    private:
        int width;
        int height;
        Vector2 cellSize;

        /// Cells are stored in a vector to increase efficiency
        std::vector<GridCell> cells;
        std::unique_ptr<IPathfindingGraph> graphAdapter;
};
