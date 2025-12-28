

#include "MultiplayerPhysicsZandbak.h"

#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Demo/PlayerMovement.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "Scene/Scene.h"

#include <memory>

std::unique_ptr<Scene> MultiplayerPhysicsZandbak::getScene()
{
	auto scene = std::make_unique<Scene>("PhysicsZandbak");

	// Static ground platform
	auto ground = std::make_unique<GameObject>();
	ground->setName("Ground");
	ground->getTransform()->setPosition({200.0f, 500.0f});
	ground->addComponent<ShapeRenderer>()->setRectangle({400, 50}).setColor(Color::orange());

	auto groundRb = ground->addComponent<RigidBody>();
	groundRb->makeStatic();
	auto groundCollider = ground->addComponent<Collider>();
	groundCollider->setRectangle({400, 50});

	// Falling dynamic box
	auto fallingBox = std::make_unique<GameObject>();
	fallingBox->addComponent<PlayerMovement>();
	fallingBox->setName("FallingBox");
	fallingBox->getTransform()->setPosition({200.0f, 100.0f});  // Above the ground
	fallingBox->addComponent<ShapeRenderer>()->setRectangle({50, 50}).setColor(Color::darkBlue());

	auto boxRb = fallingBox->addComponent<RigidBody>();
	boxRb->makeDynamic();
	auto boxCollider = fallingBox->addComponent<Collider>();
	boxCollider->setRectangle({50, 50});

	// Add to scene
	scene->addGameObject(std::move(ground));
	scene->addGameObject(std::move(fallingBox));

	return scene;  // No need for std::move on return
}