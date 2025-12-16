#include "Physics/PhysicsSystem.h"
#include "Core/GameWorld.h"

void PhysicsSystem::start(GameWorld& gameWorld)
{
	physicsWorld->initialize();
	gameWorld.physics = physicsWorld.get();
}

void PhysicsSystem::fixedUpdate(const float deltaTime,
                                const GameWorld& gameWorld)
{
	physicsWorld->step(deltaTime);
	physicsWorld->syncTransforms();
}

void PhysicsSystem::shutdown(GameWorld& gameWorld)
{
	gameWorld.physics = nullptr;
	physicsWorld->destroy();
}

const std::string PhysicsSystem::getName() const
{
	return "PhysicsSystem";
}
