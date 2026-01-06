#pragma once

#include "../../Component/BaseComponentTypes/Component.h"
#include "Math/Vector2.h"
#include "Physics/PhysicsShapes.h"

class Collider: public Component
{
   private:
	PhysicsShapeType shape = PhysicsShapeType::None;
	bool sensor = false;
	float density = 1.0f;
	float restitution = 0.0f;

   public:
	float radius = 0.0f;			 // circle
	Vector2 size = Vector2::zero();	 // box

	void setDensity(float _density)
	{
		this->density = _density;
	}

	float getDensity() const
	{
		return this->density;
	}

	void setRestitution(float _restitution)
	{
		this->restitution = _restitution;
	}

	float getRestitution() const
	{
		return this->restitution;
	}

	void setSensor(bool _sensor)
	{
		this->sensor = _sensor;
	}

	bool isSensor() const
	{
		return this->sensor;
	}

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

	PhysicsShapeType getShape() const
	{
		return shape;
	}

	/**
	 * @brief Calls all onSensorEnter on all behaviours
	 */
	void onSensorEnter(Collider* other);

	/**
	 * @brief Calls all onSensorExit on all behaviours
	 */
	void onSensorExit(Collider* other);
};