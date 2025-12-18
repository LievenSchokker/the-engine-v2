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

    if (input->wasKeyPressed(KeyCode::SPACE))
    {
       if (agent->requestPath(target->getPosition()))
       {
           if (pathRenderer != nullptr)
               pathRenderer->setPath(agent->getCurrentPath());
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



