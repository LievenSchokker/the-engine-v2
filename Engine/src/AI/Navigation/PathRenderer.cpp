
#include "AI/Navigation/Pathfinding/PathRenderer.h"

#include "AI/Navigation/Pathfinding/PathResult.h"
#include "AI/Agent.h"
#include "Scene/Scene.h"

void PathRenderer::onAwake()
{
    navigationSystem = gameObject->getScene().getNavigationSystem();

    if (navigationSystem == nullptr)
    {
        setEnabled(false);
    }
}

void PathRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    const float circleRadius = pathRenderOptions.circleRadius;

    Color circleColor = currentPath.isValid() ? pathRenderOptions.validPathColor : pathRenderOptions.invalidPathColor;
    for (const auto& node : currentPath.getPath())
    {
        RenderCommand dot;
        dot.type = RenderCommandType::Circle;

        dot.position = node;
        dot.radius = circleRadius;
        dot.color = circleColor;
        queue.push(dot);
    }
}


void PathRenderer::setPath(const PathResult &newPath)
{
    currentPath = newPath;
}


