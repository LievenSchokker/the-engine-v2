//
// Created by samle on 14/12/2025.
//
#include "../../../inc/AI/Navigation/NavigationGridRenderer.h"
#include "AI/Navigation/NavigationGrid.h"


void NavigationGridRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    if (grid == nullptr)
        return;

    Vector2 origin{0.0f, 0.0f}; // or read from scene / GameObject transform
    Vector2 cellSize = grid->getCellSize();
    int width = grid->getWidth();
    int height = grid->getHeight();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            GridCell cell = grid->getCell(x, y);
            RenderCommand cmd;
            cmd.type = RenderCommandType::Rectangle;
            cmd.position = origin + Vector2{x * cellSize.x + cellSize.x/2, y * cellSize.y + cellSize.y/2};
            cmd.size = cellSize;
            cmd.color = cell.walkable ? Color{0,255,0,100} : Color{255,0,0,100};
            cmd.layer = 0;
            queue.push(cmd);
        }
    } if (grid == nullptr)
        return;

    Vector2 origin{0.0f, 0.0f}; // or read from scene / GameObject transform
    Vector2 cellSize = grid->getCellSize();
    int width = grid->getWidth();
    int height = grid->getHeight();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            GridCell cell = grid->getCell(x, y);
            RenderCommand cmd;
            cmd.type = RenderCommandType::Rectangle;
            cmd.position = origin + Vector2{x * cellSize.x + cellSize.x/2, y * cellSize.y + cellSize.y/2};
            cmd.size = cellSize;
            cmd.color = cell.walkable ? Color{0,255,0,100} : Color{255,0,0,100};
            cmd.layer = 0;
            queue.push(cmd);
        }
    }
}

void NavigationGridRenderer::setGrid( NavigationGrid& navigationGrid)
{
    grid = &navigationGrid;
}

