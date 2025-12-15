//
// Created by samle on 10/12/2025.
//
#include "NavigationTest.h"

#include <iostream>
#include <ostream>

#include "AI/Agent.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Component/GridComponent.h"
#include "Component/Transform.h"
#include "Input/InputManager.h"
#include "AI/Navigation/Pathfinding/PathRenderer.h"
#include "Scene/Scene.h"

void NavigationTest::onAwake()
{
    input = InputManager::getInstance();

    auto& scene = gameObject->getScene();

    if (gameObject->hasComponent<PathRenderer>())
    {
        pathRenderer = gameObject->getComponent<PathRenderer>();
        std::cout << "Has path renderer" << std::endl;
    }
}

void NavigationTest::update(float deltaTime, GameWorld *world)
{
    if (input == nullptr)
        return;

    if (input->wasKeyPressed(KeyCode::SPACE))
    {
       if (agent->requestPath(target->getPosition()))
       {
            std::cout << "Agent requested path success" << std::endl;
            pathRenderer->setPath(agent->getPathResult().getPath());
       }
        else
        {
            std::cout << "Agent requested path failed" << std::endl;
        }
    }
}

void NavigationTest::setTarget(const Transform &targetTransform)
{
    target = &targetTransform;
}

void NavigationTest::setAgent(Agent& newAgent)
{
    agent = &newAgent;
}



