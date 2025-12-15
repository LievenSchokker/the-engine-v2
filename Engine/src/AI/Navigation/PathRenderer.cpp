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

        if (navigationSystem != nullptr)
        {
            navSurface = navigationSystem->getNavigationSurface();

            if (navSurface == nullptr)
            {
                std::cout << "Navigation surface is not found " << std::endl;
                setEnabled(false);
            }
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

    player = agent->getGameObject()->getScene().getGameObject("Player");
    if (player == nullptr)
        return;

    target = player->getTransform()->getPosition();

    PathResult pathResult;
    if (agent->tryGetPath(target, &pathResult))
    {
        setPath(pathResult.getPath());
    }
}

void PathRenderer::fillRenderQueue(IRenderQueueWriter &queue) const
{
    if (path.empty())
        return;

    const float circleRadius = 3;

    for (const auto& node : path)
    {
        RenderCommand dot;
        dot.type = RenderCommandType::Circle;

        dot.position = navSurface->toWorldPoint(node);
        dot.radius = circleRadius;
        dot.color = Color::darkOrange();
        queue.push(dot);
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