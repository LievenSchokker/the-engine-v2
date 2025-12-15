#include "Behaviour/PhysicsUpdateBehaviour.h"

#include "Physics/IPhysicsWorld.h"

PhysicsUpdateBehaviour::PhysicsUpdateBehaviour(IPhysicsWorld* physicsWorld)
	: physicsWorld(physicsWorld)
{
}

void PhysicsUpdateBehaviour::fixedUpdate()
{
	if ( physicsWorld )
	{
		physicsWorld->fixedUpdate();
	}
}
