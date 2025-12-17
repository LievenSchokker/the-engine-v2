//
// Created by samle on 26/11/2025.
//
#include "SimpleMoveBehaviour.h"

#include "Component/Transform.h"

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

void SimpleMoveBehaviour::update(double deltaTime, const GameWorld& gameWorld)
{
	pollInput();

	if ( canMove ) move();
}

void SimpleMoveBehaviour::onDisable()
{
	std::cout << "SimpleMoveBehaviour is now disabled" << std::endl;
	currentSpeed = 0;
}

void SimpleMoveBehaviour::move()
{
	Vector2 newPos = transform->getPosition() + currentDirection * currentSpeed;
	transform->setPosition(newPos);
}

void SimpleMoveBehaviour::pollInput()
{
	if (inputManager == nullptr)
	    return;

	currentDirection = {0.0f, 0.0f};

	if (inputManager->isKeyDown(KeyCode::W))
	    currentDirection.y -= 0.01f;
	if (inputManager->isKeyDown(KeyCode::S))
	    currentDirection.y += 0.01f;
	if (inputManager->isKeyDown(KeyCode::A))
	    currentDirection.x -= 0.01f;
	if (inputManager->isKeyDown(KeyCode::D))
	    currentDirection.x += 0.01f;

	float length = std::sqrt(currentDirection.x * currentDirection.x +
	currentDirection.y * currentDirection.y); if (length > 0.0f)
	{
	    currentDirection.x /= length;
	    currentDirection.y /= length;
	}
}
