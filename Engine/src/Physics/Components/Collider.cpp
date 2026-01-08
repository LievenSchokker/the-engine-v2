#include "Behaviour/Behaviour.h"
#include "Physics/Components/Collider.h"

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

void Collider::serialize(WriteArchive& archive) const
{
	int shapeInt = static_cast<int>(shape);
	archive.process(shapeInt);
	archive.process(const_cast<float&>(radius));

	float sizeX = size.x;
	float sizeY = size.y;
	archive.process(sizeX);
	archive.process(sizeY);

	archive.process(const_cast<bool&>(sensor));
	archive.process(const_cast<float&>(density));
}

void Collider::deserialize(ReadArchive& archive)
{
	int shapeInt;
	archive.process(shapeInt);
	shape = static_cast<PhysicsShapeType>(shapeInt);

	archive.process(radius);

	float sizeX, sizeY;
	archive.process(sizeX);
	archive.process(sizeY);
	size = Vector2(sizeX, sizeY);

	archive.process(sensor);
	archive.process(density);
}
