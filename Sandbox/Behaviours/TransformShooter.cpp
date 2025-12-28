//
// Created by Lieven Schokker on 20/12/2025.
//

#include "TransformShooter.h"

#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/GameWorld.h"
#include "Scene/SceneManager.h"

void TransformShooter::onAwake()
{
}

void TransformShooter::onEnable()
{
}

void TransformShooter::onStart()
{

}

void TransformShooter::onDisable()
{

}

void TransformShooter::update(double deltaTime, const GameWorld& world)
{

	inputManager = world.input;
	if (world.sceneManager != nullptr)
	{
		activeScene = world.sceneManager->getActiveScene();
	}
	if (inputManager == nullptr)
		return;

	spawnGameObject();

	for(auto transform : listOfTransforms )
	{
		transform->rotateTowards(getTransform()->getPosition(), 100, deltaTime);
	}
}

void TransformShooter::spawnGameObject()
{
	if (inputManager->isKeyDown(KeyCode::SPACE))
	{
		auto gameObject = std::make_unique<GameObject>();
		auto transform = gameObject->getTransform();
		listOfTransforms.push_back(transform);
		transform->setPosition(getTransform()->getPosition());
		transform->setRotationAngle(getTransform()->getRotationAngle());
		auto shape = gameObject->addComponent<ShapeRenderer>();
		shape->setRectangle(Vector2{20, 20});
		activeScene->addGameObject(std::move(gameObject));
	}
}

