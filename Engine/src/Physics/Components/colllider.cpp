#include "Physics/Components/Collider.h"

void Collider::onSensorExit(Collider* other)
{
	for ( auto* behaviour : gameObject->getAllBehaviours() )
	{
		if ( behaviour->getIsActiveAndEnabled() )
			behaviour->onSensorExit(other);
	}
}


void Collider::onSensorEnter(Collider* other)
{
	// Notify all behaviours on this GameObject
	for ( auto* behaviour : gameObject->getAllBehaviours() )
	{
		if ( behaviour->getIsActiveAndEnabled() )
			behaviour->onSensorEnter(other);
	}
}