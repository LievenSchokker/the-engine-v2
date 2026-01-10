
#include "AI/Navigation/Pathfinding/PathRenderer.h"

#include "AI/Navigation/Pathfinding/PathResult.h"
#include "AI/Agent.h"
#include "GameObject/GameObject.h"
#include "Rendering/RenderUtils.h"
#include "Scene/Scene.h"

void PathRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    if (!renderEnabled)
        return;

    const float circleRadius = pathRenderOptions.circleRadius;
    const Color circleColor = currentPath.isValid() ? pathRenderOptions.validPathColor : pathRenderOptions.invalidPathColor;
    const uint8_t renderLayer = clampRenderLayer(getGameObject());

    for (const auto& node : currentPath.getPath())
    {
        RenderCommand command;
        command.type = RenderCommandType::Circle;

        command.position = node;
        command.radius = circleRadius;
        command.color = circleColor;
        command.layer = renderLayer;
        command.orderInLayer = orderInLayer;
        queue.push(command);
    }
}


void PathRenderer::setPath(const PathResult &newPath)
{
    currentPath = newPath;
}
