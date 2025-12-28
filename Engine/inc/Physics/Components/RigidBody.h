#pragma once


#include "Behaviour/Behaviour.h"
#include "Core/GameWorld.h"
#include "Networking/Serialization/RegistrationBase.h"
#include "Physics/IPhysicsWorld.h"

class RigidBody: public Behaviour, public RegistrationBase<RigidBody>
{
public:
	static constexpr const char* name()
	{
		return "RigidBody";
	}
	const char* getName() const override { return name(); }

	bool isDynamic = true;

	void makeStatic()
	{
		isDynamic = false;
	}

	void makeDynamic()
	{
		isDynamic = true;
	}


	void onStart() override
	{
		std::cout << "[RigidBody::onStart] Called" << std::endl;
		std::cout << "[RigidBody::onStart] gameWorld = " << gameWorld << std::endl;

		if (gameWorld)
		{
			std::cout << "[RigidBody::onStart] physics = " << gameWorld->physics << std::endl;

			if (gameWorld->physics)
			{
				std::cout << "[RigidBody::onStart] Creating body..." << std::endl;
				gameWorld->physics->createBody(this);
			}
			else
			{
				std::cout << "[RigidBody::onStart] ERROR: physics is null!" << std::endl;
			}
		}
		else
		{
			std::cout << "[RigidBody::onStart] ERROR: gameWorld is null!" << std::endl;
		}
	}

	void onDestroy() override
	{
		// Unregister from physics world when destroyed
		if (gameWorld && gameWorld->physics)
		{
			gameWorld->physics->destroyBody(this);
		}
	}

	void fixedUpdate()
	{
	}
};