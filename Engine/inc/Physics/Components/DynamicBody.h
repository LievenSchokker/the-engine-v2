#pragma once

#include "Component/Component.h"
#include "GameObject/Vector2.h"

class DynamicBody : public Component
{
	public:
		DynamicBody(bool useGravity, Vector2 velocity, float mass);
		~DynamicBody() override;
	private:
		bool useGravity = false;
		Vector2 velocity;
		float mass;
};