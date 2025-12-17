#pragma once


#include "../../Component/BaseComponentTypes/Component.h"
#include "Physics/PhysicsShapes.h"


class Collider: public Component
{
public:
	PhysicsShapeType shape = PhysicsShapeType::None;

	float radius = 0.0f; // circle
	Vector2 size = {0, 0}; // box

	float density = 1.0f;
	bool isSensor = false;

	void setCircle(const float radiusNew)
	{
		shape = PhysicsShapeType::Circle;
		radius = radiusNew;
	}

	void setRectangle(const Vector2& fullSize)
	{
		shape = PhysicsShapeType::Rectangle;
		size = fullSize;
	}
};