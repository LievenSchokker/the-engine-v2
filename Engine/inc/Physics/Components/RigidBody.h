//
// Created by thijs on 24-11-2025.
//


#pragma once
#pragma once
#include "Component/Component.h"


class RigidBody: public Component
{
public:
	bool isDynamic = true;
	b2BodyId bodyId = b2_nullBodyId;

	void makeStatic()
	{
		isDynamic = false;
	}

	void makeDynamic()
	{
		isDynamic = true;
	}
};