//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationCell.h"
#include "Math/Vector2.h"


void NavigationGrid::initialise(Vector2 dimensions)
{
    width = dimensions.x;
    height = dimensions.y;
    cells.resize(width * height);
}

bool NavigationGrid::isWalkable(Vector2 cellPosition) const
{
    if (!isValidCell(cellPosition))
        return false;

    return cells[cellPosition.y * width + cellPosition.x].walkable;
}

float NavigationGrid::getCellWeight(Vector2 cellPosition)
{
    if (!isValidCell(cellPosition))
        return false;

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


bool NavigationGrid::isValidCell(Vector2 position) const
{
    return position.x >= 0 && position.y >= 0 && position.x < width && position.y < height;
}
