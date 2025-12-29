#pragma once

#include "Behaviour/Behaviour.h"

class RigidBody: public Behaviour
{
   public:
	bool isDynamic = true;

	void makeStatic()
	{
		isDynamic = false;
	}

	void makeDynamic()
	{
		isDynamic = true;
	}

   protected:
	/**
	 * @brief Creates the physics body when the RigidBody behaviour starts.
	 *
	 * This is called automatically by the engine when the behaviour is
	 * initialized.
	 */
	void onStart() override;

	/**
	 * @brief Destroys the physics body when the RigidBody component is
	 * destroyed.
	 *
	 * This is called automatically by the engine when the component is
	 * destroyed.
	 */
	void onDestroy() override;
};