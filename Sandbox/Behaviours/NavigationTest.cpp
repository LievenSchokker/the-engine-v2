//
// Created by samle on 10/12/2025.
//
#include "NavigationTest.h"

#include <iostream>
#include <ostream>

#include "AI/Navigation/NavigationSystem.h"
#include "Component/GridComponent.h"
#include "Component/Transform.h"
#include "Input/InputManager.h"
#include "Scene/Scene.h"

void NavigationTest::onAwake()
{
    input = InputManager::getInstance();

    auto& scene = gameObject->getScene();
    navSystem = scene.getNavigationSystem();
    grid = scene.getAllComponentsOfType<NavigationGrid>().front();
}

void NavigationTest::update(float deltaTime, GameWorld *world)
{
    if (input == nullptr)
        return;

    if (input->wasKeyPressed(KeyCode::ENTER))
    {
        if (target == nullptr)
            return;

        createPath();
    }
    if (input->wasKeyPressed(KeyCode::SPACE))
    {
        showPath();
    }
    //
    // if (!pathResult.isValid())
    //     return;
    //
    // if (Vector2::distance(transform->getPosition(), pathResult.getPath().at(currentWaypointIndex)) < 5)
    //     currentWaypointIndex++;
    //
    // if (!pathResult.getPath().empty())
    // {
    //     transform->moveTowards(pathResult.getPath().at(currentWaypointIndex), 10);
    // }
}

void NavigationTest::setTarget(const Transform &targetTransform)
{
    target = &targetTransform;
}


void NavigationTest::createPath()
{
    if (navSystem == nullptr)
    {
        std::cout << "NavSystem is null" << std::endl;
        return;
    }

    if (grid == nullptr)
    {
        std::cout << "Grid is null" << std::endl;
        return;
    }

    pathResult = navSystem->computePath(transform->getPosition(), target->getPosition());
    std::cout << " Created path! " << pathResult.getPathSize() << std::endl;
}


void NavigationTest::showPath()
{
    showPathDebug = !showPathDebug;
    std::cout << "Show Path: " << showPathDebug << std::endl;
}

void NavigationTest::fillRenderQueue(IRenderQueueWriter &queue) const
{
    const float spacing = 0.05f; // small gap between cells

    const std::vector<Vector2>& path = pathResult.getPath();
    if (path.empty())
    {
        return;
    }

    for (size_t i = 0; i < path.size(); ++i)
    {
        RenderCommand cmd;
        cmd.type = RenderCommandType::Rectangle;


        Vector2 worldPos = grid->cellToWorldPosition(path[i]);
        worldPos = worldPos - (grid->getCellSize() * 0.5f); // adjust top-left for rendering

        cmd.position = worldPos;
        cmd.size = grid->getCellSize() - Vector2{spacing, spacing};
        cmd.layer = 101; // slightly above the grid

        // Color logic
        if (i == 0)
            cmd.color = Color::blue(); // start
        else if (i == path.size() - 1)
            cmd.color = Color::red();  // end
        else
            cmd.color = Color::yellow(75); // path

        queue.push(cmd);
    }
}


