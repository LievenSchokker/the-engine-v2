
#include "AI/Navigation/NavigationGridRenderer.h"
#include "AI/Navigation/NavigationGrid.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"

void NavigationGridRenderer::onAwake()
{
    grid = dynamic_cast<NavigationGrid*>(gameObject->getScene().getNavigationSystem()->getNavigationSurface());

    if (grid == nullptr)
    {
        setEnabled(false);
        return;
    }

    buildCache();
}


void NavigationGridRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    if (!cacheBuilt)
        return;

    for (const auto& cmd : cachedCommands)
    {
        RenderCommand command = cmd;
        queue.push(command);
    }
}

void NavigationGridRenderer::buildCache()
{
     cachedCommands.clear();

    if (!grid)
        return;

    const Vector2 cellSize = grid->getCellSize();
    const int width = grid->getWidth();
    const int height = grid->getHeight();

    const float lineThickness = gridRenderOptions.lineThickness;
    Color walkableColor = gridRenderOptions.walkableCellsColor;
    Color unwalkableColor = gridRenderOptions.unwalkableCellsColor;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Vector2 center{
                x * cellSize.x + cellSize.x * 0.5f,
                y * cellSize.y + cellSize.y * 0.5f
            };

            bool currentCellWalkable = grid->isWalkable({(float)x, (float)y});

            // Right edge: only draw for last column
            if (x == width - 1)
            {
                cachedCommands.push_back(RenderCommand{
                    .type = RenderCommandType::Rectangle,
                    .position = { center.x + cellSize.x * 0.5f, center.y },
                    .size = { lineThickness, cellSize.y },
                .color = (currentCellWalkable && grid->isWalkable(Vector2{(float)x, (float)y-1})) ? walkableColor : unwalkableColor
                });
            }

            // Bottom edge: only draw for last row
            if (y == height - 1)
            {
                cachedCommands.push_back(RenderCommand{
                    .type = RenderCommandType::Rectangle,
                    .position = { center.x, center.y + cellSize.y * 0.5f },
                    .size = { cellSize.x, lineThickness },
                    .color = (currentCellWalkable &&grid->isWalkable(Vector2{(float)x, (float)y+1})) ? walkableColor : unwalkableColor
                });
            }

            // Top edge
            cachedCommands.push_back(RenderCommand{
                .type = RenderCommandType::Rectangle,
                .position = { center.x, center.y - cellSize.y * 0.5f },
                .size = { cellSize.x, lineThickness },
                .color = (currentCellWalkable && grid->isWalkable(Vector2{(float)x, (float)y-1})) ? walkableColor : unwalkableColor
            });

            // Left edge (always)
            cachedCommands.push_back(RenderCommand{
                .type = RenderCommandType::Rectangle,
                .position = { center.x - cellSize.x * 0.5f, center.y },
                .size = { lineThickness, cellSize.y },
                .color = (currentCellWalkable && grid->isWalkable(Vector2{(float)x-1, (float)y})) ? walkableColor : unwalkableColor
            });
        }
    }

    cacheBuilt = true;
}


void NavigationGridRenderer::setGrid( NavigationGrid& navigationGrid)
{
    grid = &navigationGrid;
    buildCache();
}
