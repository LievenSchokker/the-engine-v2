#include "Physics/Components/RigidBody.h"

#include "Component/NetworkIdentity.h"
#include "Core/GameWorld.h"
#include "Physics/IPhysicsWorld.h"

void RigidBody::serialize(WriteArchive& archive) const
{
	Component::serialize(archive);

	uint8_t bodyTypeValue = static_cast<uint8_t>(bodyType);
	archive.process(bodyTypeValue);

	bool fixed = fixedRotation;
	archive.process(fixed);

	float damping = linearDamping;
	archive.process(damping);

	float gravity = gravityScale;
	archive.process(gravity);

	bool bullet = isBullet;
	archive.process(bullet);

	archive.process(isDynamic);
	archive.process(linearVelocity.x);
	archive.process(linearVelocity.y);
	archive.process(angularVelocity);
}

void RigidBody::deserialize(ReadArchive& archive)
{
	Component::deserialize(archive);

	uint8_t bodyTypeValue = 0;
	archive.process(bodyTypeValue);

	bool fixed = false;
	archive.process(fixed);

	float damping = 0.0f;
	archive.process(damping);

	float gravity = 1.0f;
	archive.process(gravity);

	bool bullet = false;
	archive.process(bullet);

	archive.process(isDynamic);
	archive.process(linearVelocity.x);
	archive.process(linearVelocity.y);
	archive.process(angularVelocity);

	bodyType = static_cast<BodyType>(bodyTypeValue);
	fixedRotation = fixed;
	linearDamping = damping;
	gravityScale = gravity;
	isBullet = bullet;
}

void RigidBody::onStart()
{
	// Create physics body when the RigidBody behaviour starts
	GameWorld* world = getWorld();
	if ( world != nullptr && world->physics != nullptr )
	{
		if ( world->isClient() && !world->isServer() )
		{
			auto* identity = getComponent<NetworkIdentity>();
			if ( identity != nullptr )
			{
				const int ownerId = identity->getOwnerId();
				if ( ownerId < 0 || ownerId != world->localClientId )
				{
					if ( bodyType == BodyType::Dynamic )
					{
						// Avoid client simulation fighting server snapshots.
						bodyType = BodyType::Kinematic;
					}
				}
			}
		}

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
