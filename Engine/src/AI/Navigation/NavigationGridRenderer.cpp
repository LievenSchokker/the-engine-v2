//
// Created by samle on 14/12/2025.
//
#include "../../../inc/AI/Navigation/NavigationGridRenderer.h"
#include "AI/Navigation/NavigationGrid.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"

void NavigationGridRenderer::onAwake()
{
    grid = dynamic_cast<NavigationGrid*>(gameObject->getScene().getNavigationSystem()->getNavigationSurface());

    if (grid == nullptr)
    {
        std::cout << "Failed to load navigation grid" << std::endl;
        setEnabled(false);
    }
}


void NavigationGridRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    if (!grid)
        return;

    const Vector2 origin = transform->getPosition();
    const Vector2 cellSize = grid->getCellSize();

    const int width = grid->getWidth();
    const int height = grid->getHeight();

    const float dotRadius = std::min(cellSize.x, cellSize.y) * 0.1f;
    const float lineThickness = std::min(cellSize.x, cellSize.y) * 0.05f;

    // Collect all walkable cells
    std::vector<Vector2> walkableCenters;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (!grid->isWalkable({std::floorf(x), std::floorf(y)}))
                continue;

            Vector2 cellCenter{
                origin.x + x * cellSize.x + cellSize.x / 2.0f,
                origin.y + y * cellSize.y + cellSize.y / 2.0f
            };

            walkableCenters.push_back(cellCenter);
        }
    }

    // Draw connections as thin rectangles
    for (const auto& center : walkableCenters)
    {
        // Simple 4-directional neighbors
        std::vector<Vector2> neighbors = {
            {center.x + cellSize.x, center.y},
            {center.x - cellSize.x, center.y},
            {center.x, center.y + cellSize.y},
            {center.x, center.y - cellSize.y}
        };

        for (const auto& neighbor : neighbors)
        {
            // Only draw if neighbor exists in walkableCenters
            if (std::find(walkableCenters.begin(), walkableCenters.end(), neighbor) != walkableCenters.end())
            {
                Vector2 lineCenter{ (center.x + neighbor.x) / 2.0f, (center.y + neighbor.y) / 2.0f };
                Vector2 lineSize{ lineThickness, lineThickness };

                // If horizontal, width = distance; vertical, height = distance
                if (center.x != neighbor.x)
                    lineSize.x = std::abs(neighbor.x - center.x);
                else
                    lineSize.y = std::abs(neighbor.y - center.y);

                RenderCommand line;
                line.type = RenderCommandType::Rectangle;
                line.position = lineCenter;
                line.size = lineSize;
                line.color = Color{200, 200, 200, 255};
                queue.push(line);
            }
        }
    }

    // Draw dots for walkable cells
    for (const auto& center : walkableCenters)
    {
        RenderCommand dot;
        dot.type = RenderCommandType::Circle;
        dot.position = center;
        dot.radius = dotRadius;
        dot.color = Color{0, 255, 0, 255};
        queue.push(dot);
    }
}


void NavigationGridRenderer::setGrid( NavigationGrid& navigationGrid)
{
    grid = &navigationGrid;
}
