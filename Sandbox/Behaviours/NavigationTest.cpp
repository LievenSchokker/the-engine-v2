//
// Created by samle on 10/12/2025.
//
#include "NavigationTest.h"

#include <iostream>
#include <ostream>
#include <random>
#include <vector>

#include "AI/Agent.h"
#include "Component/GridComponent.h"
#include "Component/Transform.h"
#include "Input/InputManager.h"
#include "AI/Navigation/Pathfinding/PathRenderer.h"
#include "Scene/Scene.h"

namespace
{
std::mt19937& navigationTestRng()
{
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

bool tryPickRandomWalkableCell(const GridComponent& grid,
                               const Vector2* avoidA,
                               const Vector2* avoidB,
                               Vector2& outCell)
{
    const int width = grid.getGridWidth();
    const int height = grid.getGridHeight();
    if (width <= 0 || height <= 0)
        return false;

    std::vector<Vector2> candidates;
    candidates.reserve(static_cast<size_t>(width * height));

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Vector2 cell{static_cast<float>(x), static_cast<float>(y)};
            if (!grid.isWalkable(cell))
                continue;
            if (avoidA != nullptr && cell == *avoidA)
                continue;
            if (avoidB != nullptr && cell == *avoidB)
                continue;
            candidates.push_back(cell);
        }
    }

    if (candidates.empty())
        return false;

    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    outCell = candidates[dist(navigationTestRng())];
    return true;
}

bool requestPathForAgent(Agent& agent, const Transform& target, PathRenderer* renderer)
{
    if (!agent.requestPath(target.getPosition()))
        return false;

    if (renderer != nullptr)
        renderer->setPath(agent.getCurrentPath());

    return true;
}
} // namespace

void NavigationTest::onAwake()
{
    if (gameObject->hasComponent<PathRenderer>())
    {

        pathRenderer = gameObject->getComponent<PathRenderer>();
    }
    else
    {
        std::cout << "[NavigationTest component] No PathRenderer component exists on this GameObbject, consider adding it." << std::endl;
    }
}

void NavigationTest::update(double deltaTime, const GameWorld& world)
{
	input = world.input;
    if (input == nullptr)
        return;

    if (agent == nullptr || target == nullptr)
        return;

    if (input->wasKeyPressed(KeyCode::SPACE))
        requestPathForAgent(*agent, *target, pathRenderer);

    if (gridComponent == nullptr || !agent->hasPath())
    {
        wasAtTarget = false;
        return;
    }

    const Transform* agentTransform = agent->getTransform();
    if (agentTransform == nullptr)
    {
        wasAtTarget = false;
        return;
    }

    const float arrivalDistance = agent->getArrivingDistance();
    const float distanceToTarget =
        Vector2::distance(agentTransform->getPosition(), target->getPosition());
    const bool atTarget = distanceToTarget <= arrivalDistance;

    if (atTarget && !wasAtTarget)
    {
        const Vector2 currentTargetCell =
            gridComponent->worldToCell(target->getPosition());
        const Vector2 agentCell =
            gridComponent->worldToCell(agentTransform->getPosition());
        Vector2 newTargetCell = Vector2::zero();

        if (tryPickRandomWalkableCell(*gridComponent, &currentTargetCell,
                                      &agentCell, newTargetCell) ||
            tryPickRandomWalkableCell(*gridComponent, &currentTargetCell, nullptr,
                                      newTargetCell))
        {
            if (newTargetCell != currentTargetCell)
            {
                target->setPosition(gridComponent->cellToWorld(newTargetCell));
                requestPathForAgent(*agent, *target, pathRenderer);
            }
        }
    }

    wasAtTarget = atTarget;
}

void NavigationTest::setTarget(Transform &targetTransform)
{
    target = &targetTransform;
    wasAtTarget = false;
}

void NavigationTest::setAgent(Agent& newAgent)
{
    agent = &newAgent;
}

void NavigationTest::setGridComponent(GridComponent& grid)
{
    gridComponent = &grid;
}

