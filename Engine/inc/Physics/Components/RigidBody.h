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
	void onStart() override;
	void onDestroy() override;
};