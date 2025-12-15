//
// Created by samle on 14/12/2025.
//
#include "AI/Navigation/Pathfinding/PathRenderer.h"

#include "AI/Navigation/Pathfinding/PathResult.h"
#include "AI/Agent.h"
#include "Scene/Scene.h"

void PathRenderer::onAwake()
{
    navigationSystem = gameObject->getScene().getNavigationSystem();

    if (navigationSystem == nullptr)
    {
        std::cout << "No nav system found";
        setEnabled(false);
    }
}

void PathRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    if (path.empty())
        return;

    const float circleRadius = pathRenderOptions.circleRadius;

    for (const auto& node : path)
    {
        RenderCommand dot;
        dot.type = RenderCommandType::Circle;

        dot.position = navigationSystem->getNavigationSurface()->toWorldPoint(node);
        dot.radius = circleRadius;
        dot.color = pathRenderOptions.circleColor;
        queue.push(dot);
    }
}


void PathRenderer::setPath(const std::vector<Vector2>& newPath)
{
    path = newPath;
}
