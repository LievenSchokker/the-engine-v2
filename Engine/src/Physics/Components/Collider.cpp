#include "Behaviour/Behaviour.h"
#include "Physics/Components/Collider.h"

void Collider::serialize(WriteArchive& archive) const
{
	uint8_t shapeType = static_cast<uint8_t>(shape);
	archive.process(shapeType);

	bool isSensor = sensor;
	archive.process(isSensor);

	float densityValue = density;
	float restitutionValue = restitution;
	archive.process(densityValue);
	archive.process(restitutionValue);

	float radiusValue = radius;
	float sizeX = size.x;
	float sizeY = size.y;
	archive.process(radiusValue);
	archive.process(sizeX);
	archive.process(sizeY);
}

void Collider::deserialize(ReadArchive& archive)
{
	uint8_t shapeType = 0;
	archive.process(shapeType);

	bool isSensor = false;
	archive.process(isSensor);

	float densityValue = 1.0f;
	float restitutionValue = 0.0f;
	archive.process(densityValue);
	archive.process(restitutionValue);

	float radiusValue = 0.0f;
	float sizeX = 0.0f;
	float sizeY = 0.0f;
	archive.process(radiusValue);
	archive.process(sizeX);
	archive.process(sizeY);

	setSensor(isSensor);
	setDensity(densityValue);
	setRestitution(restitutionValue);

	const PhysicsShapeType newShape =
		static_cast<PhysicsShapeType>(shapeType);
	switch (newShape)
	{
		case PhysicsShapeType::Circle:
			setCircle(radiusValue);
			break;
		case PhysicsShapeType::Rectangle:
			setRectangle({sizeX, sizeY});
			break;
		case PhysicsShapeType::None:
		default:
			shape = PhysicsShapeType::None;
			radius = radiusValue;
			size = {sizeX, sizeY};
			break;
	}
}

void Collider::onSensorExit(Collider* other)
{
	if ( other == nullptr || gameObject == nullptr ) return;

	for ( auto* behaviour : gameObject->getAllBehaviours() )
	{
		if ( behaviour && behaviour->getIsActiveAndEnabled() )
			behaviour->onSensorExit(other);
	}
}

void Collider::onSensorEnter(Collider* other)
{
	if ( other == nullptr || gameObject == nullptr ) return;

	// Notify all behaviours on this GameObject
	for ( auto* behaviour : gameObject->getAllBehaviours() )
	{
		if ( behaviour && behaviour->getIsActiveAndEnabled() )
			behaviour->onSensorEnter(other);
	}
}
