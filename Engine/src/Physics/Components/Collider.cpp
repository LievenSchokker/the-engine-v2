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