#pragma once


#include "box2d/id.h"
#include "Physics/IPhysicsWorld.h"


class Box2DPhysicsWorld: public IPhysicsWorld
{
public:
	Box2DPhysicsWorld();
	~Box2DPhysicsWorld() override = default;
	void start() override;
	void update() override;
	void shutdown() override;
	b2BodyId createBody(const GameObject* gameObject) override;
	void destroyBody(b2BodyId body);

private:
	b2WorldId worldId;
};