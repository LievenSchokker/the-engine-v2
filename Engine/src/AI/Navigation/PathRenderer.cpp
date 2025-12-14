//
// Created by samle on 14/12/2025.
//
#include "../../../inc/AI/Navigation/Pathfinding/PathRenderer.h"

#include "../../../inc/AI/Navigation/Pathfinding/PathResult.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Input/InputManager.h"
#include "Scene/Scene.h"
#include "AI/Agent.h"


void PathRenderer::onAwake()
{
    if (gameObject->tryGetComponent<Agent>(agent))
    {
        navigationSystem = gameObject->getScene().getNavigationSystem();

        if (navigationSystem == nullptr)
        {
            std::cout << "Navigation System is not found " << std::endl;
            setEnabled(false);
        }
    }
    else
        std::cout << "Agent is not found on PathRenderer  " << std::endl;
}


void PathRenderer::update(float deltaTime, GameWorld *world)
{
    if (InputManager::getInstance()->wasKeyPressed(KeyCode::SPACE))
    {
        computeNewPath();
    }
}

void PathRenderer::computeNewPath()
{
    if (!agent)
        return;


    PathResult pathResult;
    if (agent->tryGetPath(target, &pathResult))
    {
        setPath(pathResult.getPath());
    }
}

void PathRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    for (size_t i = 0; i < path.size(); ++i)
    {
        // Draw dot
        RenderCommand dot;
        dot.type = RenderCommandType::Circle;
        dot.position = path[i];
        dot.radius = 5.0f;
        dot.color = Color{0, 0, 255, 255};
        queue.push(dot);

        // Draw line as thin rectangle
        if (i + 1 < path.size())
        {
            Vector2 start = path[i];
            Vector2 end = path[i + 1];
            Vector2 delta = end - start;

            RenderCommand line;
            line.type = RenderCommandType::Rectangle;
            line.position = start + delta * 0.5f;   // midpoint
            line.size = { std::sqrt(delta.x * delta.x + delta.y * delta.y), 2.0f }; // width = length, height = 2px
            line.rotationDegrees = std::atan2(delta.y, delta.x) * 180.0 / 3.14159265358979323846;
            line.color = Color{0, 0, 255, 200};
            queue.push(line);
        }
    }
}


void PathRenderer::setPath(const std::vector<Vector2>& newPath)
{
    path = newPath;
}


void PathRenderer::setTarget(const Transform &t)
{
    target = t.getPosition();
}