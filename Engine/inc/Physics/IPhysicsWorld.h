#pragma once
#include "GameObject/GameObject.h"

struct b2BodyId;

class IPhysicsWorld
{
	public:
	virtual ~IPhysicsWorld() = default;

	virtual void start() = 0;
	virtual void update() = 0;
	virtual void shutdown() = 0;
	virtual b2BodyId createBody(const GameObject* gameObject) =0;
};