#pragma once

#include "Behaviour/Behaviour.h"

class IPhysicsWorld;

/**
 * @brief Behavior class that updates the physics world simulation.
 *
 * Calls physicsWorld->fixedUpdate() during the engine's fixed update loop
 * to step the physics simulation forward and synchronize transforms.
 */
class PhysicsUpdateBehaviour: public Behaviour
{
   public:
	explicit PhysicsUpdateBehaviour(IPhysicsWorld* physicsWorld);

	~PhysicsUpdateBehaviour() override = default;

	void fixedUpdate() override;

   private:
	IPhysicsWorld* physicsWorld;
};
