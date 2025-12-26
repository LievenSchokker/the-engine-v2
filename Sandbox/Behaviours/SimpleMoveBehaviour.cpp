//
// Created by samle on 26/11/2025.
//
#include "SimpleMoveBehaviour.h"

#include "Component/Transform.h"
#include "Core/GameWorld.h"
#include "Scene/SceneManager.h"

void SimpleMoveBehaviour::onAwake()
{
	canMove = true;
}

void SimpleMoveBehaviour::onEnable()
{
	currentSpeed = maxSpeed;
}

void SimpleMoveBehaviour::onStart()
{
	std::cout << "SimpleMoveBehaviour is now started" << std::endl;
}

void SimpleMoveBehaviour::update(double deltaTime, const GameWorld& gameWorld)
{
	inputManager = gameWorld.input;
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

    if (inputManager->isKeyDown(KeyCode::H))
    {
        if (gameObject != nullptr)
        {
            if (clone != nullptr)
            {
                clone->destroy();
                clone = nullptr;
            }
            std::unique_ptr<GameObject> clone = gameObject->clone();
        }
    }

    if (inputManager->isKeyDown(KeyCode::L))
    {
        if (clone != nullptr)
        {
            getWorld()->sceneManager->getActiveScene()->addGameObject(std::move(clone));
            clone = nullptr;
            gameObject->destroy();
        }
    }

	if (inputManager->isKeyDown(KeyCode::W))
	    currentDirection.y -= 1.0f;
	if (inputManager->isKeyDown(KeyCode::S))
	    currentDirection.y += 1.0f;
	if (inputManager->isKeyDown(KeyCode::A))
	    currentDirection.x -= 1.0f;
	if (inputManager->isKeyDown(KeyCode::D))
	    currentDirection.x += 1.0f;

    currentDirection = currentDirection.normalised();
}
