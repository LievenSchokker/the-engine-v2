#include "Physics/PhysicsSystem.h"
#include "Core/GameWorld.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"

PhysicsSystem::PhysicsSystem()
	: physicsWorld(std::make_unique<Box2DPhysicsWorld>())
{
}

SystemStatus PhysicsSystem::start(GameWorld& gameWorld)
{
	physicsWorld->initialize();
	gameWorld.physics = physicsWorld.get();
	return SystemStatus::Running;
}

void PhysicsSystem::fixedUpdate(const double deltaTime,
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