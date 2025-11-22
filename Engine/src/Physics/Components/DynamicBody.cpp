#include "Physics/Components/DynamicBody.h"

DynamicBody::DynamicBody(bool useGravity, Vector2 velocity, float mass) :
	velocity(velocity),
	mass(mass),
	useGravity(useGravity)
{
}

DynamicBody::~DynamicBody()
{

}




