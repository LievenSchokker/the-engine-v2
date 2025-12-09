//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationCell.h"
#include "Math/Vector2.h"


void NavigationGrid::generateGrid(Vector2 dimensions, Vector2 cellsSize)
{
    width = static_cast<int>(dimensions.x);
    height = static_cast<int>(dimensions.y);
    cells.resize(width * height);

    cellSize = cellsSize;
}

bool NavigationGrid::isWalkable(Vector2 cellPosition) const
{
    if (!isValidCell(cellPosition))
        return false;

    return cells[cellPosition.y * width + cellPosition.x].walkable;
}

int NavigationGrid::getCellWeight(Vector2 cellPosition)
{
    if (!isValidCell(cellPosition))
        return 0;

    return cells[cellPosition.y * width + cellPosition.x].weight;
}


void NavigationGrid::setWalkable(Vector2 cellPosition, bool value)
{
    if (!isValidCell(cellPosition))
        return ;

     cells[cellPosition.y * width + cellPosition.x].walkable = value;
}


void NavigationGrid::setCellWeight(Vector2 cellPosition, float weight)
{
    if (!isValidCell(cellPosition))
        return;

    cells[cellPosition.y * width + cellPosition.x].weight = weight;
}


Vector2 NavigationGrid::getCellSize() const
{
    return cellSize;
}


void NavigationGrid::setCellSize(Vector2 size)
{
    cellSize = size;
}


Vector2 NavigationGrid::worldToCellPosition(Vector2 worldPos) const
{
    int x = int(worldPos.x / cellSize.x);
    int y = int(worldPos.y / cellSize.y);

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    return Vector2{static_cast<float>(x), static_cast<float>(y) };
}


Vector2 NavigationGrid::cellToWorldPosition(Vector2 cellPos) const
{
    float worldX = cellPos.x * cellSize.x + cellSize.x * 0.5f;
    float worldY = cellPos.y * cellSize.y + cellSize.y * 0.5f;

    return Vector2{ worldX, worldY };
}

const std::vector<NavigationCell> &NavigationGrid::getCells() const
{
    return cells;
}


bool NavigationGrid::isValidCell(Vector2 position) const
{
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);

    return x >= 0 && y >= 0 && x < width && y < height;
}

int NavigationGrid::getWidth() const
{
    return width;
}

int NavigationGrid::getHeight() const
{
    return height;
}
