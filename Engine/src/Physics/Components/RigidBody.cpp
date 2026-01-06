#include "Physics/Components/RigidBody.h"

#include "Core/GameWorld.h"
#include "Physics/IPhysicsWorld.h"

void RigidBody::onStart()
{
	// Create physics body when the RigidBody behaviour starts
	GameWorld* world = getWorld();
	if ( world != nullptr && world->physics != nullptr )
	{
		world->physics->createBody(this);
	}
}

void RigidBody::onDestroy()
{
	// Destroy physics body when the RigidBody component is destroyed
	GameWorld* world = getWorld();
	if ( world != nullptr && world->physics != nullptr )
	{
		world->physics->destroyBody(this);
	}

	// Call base class onDestroy
	Behaviour::onDestroy();
}
