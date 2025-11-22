#include "Physics/PhysicsSystem.h"
#include "Physics/Components/DynamicBody.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include <vector>

PhysicsSystem::PhysicsSystem()
	: world(std::make_unique<Box2DPhysicsWorld>())
{
}


void PhysicsSystem::registerBody(DynamicBody* body)
{
	dynamicBodies.emplace_back(body);
	world->addBody(body->getTransform());
}

void PhysicsSystem::unregisterBody(DynamicBody* body)
{
	dynamicBodies.erase(std::ranges::find(dynamicBodies, body));
}

void PhysicsSystem::syncData()
{

}

Box2DPhysicsWorld* PhysicsSystem::GetWorld()
{
	return world.get();
}

void PhysicsSystem::stepPhysics(float dt)
{
	world->update();
}
