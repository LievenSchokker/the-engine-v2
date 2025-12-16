
#include "AI/Navigation/Pathfinding/PathRenderer.h"

#include "AI/Navigation/Pathfinding/PathResult.h"
#include "AI/Agent.h"
#include "Scene/Scene.h"


void PathRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    const float circleRadius = pathRenderOptions.circleRadius;
    const Color circleColor = currentPath.isValid() ? pathRenderOptions.validPathColor : pathRenderOptions.invalidPathColor;

    for (const auto& node : currentPath.getPath())
    {
        RenderCommand command;
        command.type = RenderCommandType::Circle;

        command.position = node;
        command.radius = circleRadius;
        command.color = circleColor;
        queue.push(command);
    }
}


void PathRenderer::setPath(const PathResult &newPath)
{
    currentPath = newPath;
}


