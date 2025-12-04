//
// Created by samle on 26/11/2025.
//


#include "SimpleMoveBehaviour.h"

#include <iostream>
#include <ostream>

#include "Component/Transform.h"
#include "Input/InputManager.h"


void SimpleMoveBehaviour::onAwake()
{
    canMove = true;
    inputManager = InputManager::getInstance();
    std::cout << "SimpleMoveBehaviour is now awake" << std::endl;
}

void SimpleMoveBehaviour::onEnable()
{
    std::cout << "SimpleMoveBehaviour is now enabled" << std::endl;
    currentSpeed = maxSpeed;
}

void SimpleMoveBehaviour::onStart()
{
    std::cout << "SimpleMoveBehaviour is now started" << std::endl;
}

void SimpleMoveBehaviour::update(float deltaTime)
{
	pollInput();

	if (canMove)
	    move(deltaTime);
}

void SimpleMoveBehaviour::onDisable()
{
    std::cout << "SimpleMoveBehaviour is now disabled" << std::endl;
    currentSpeed = 0;
}

void SimpleMoveBehaviour::move(float deltaTime)
{
    Vector2 newPos = transform->getPosition() + currentDirection * currentSpeed * deltaTime;
    transform->setPosition(newPos);
}

void SimpleMoveBehaviour::pollInput()
{
    if (inputManager == nullptr)
        return;

    currentDirection = {0.0f, 0.0f};

    if (inputManager->isKeyDown(KeyCode::W))
        currentDirection.setY(- 1.0f);
    if (inputManager->isKeyDown(KeyCode::S))
        currentDirection.setY(  1.0f);
    if (inputManager->isKeyDown(KeyCode::A))
        currentDirection.setX( - 1.0f);
    if (inputManager->isKeyDown(KeyCode::D))
        currentDirection.setX(1.0f);

    currentDirection.normalize();
}







