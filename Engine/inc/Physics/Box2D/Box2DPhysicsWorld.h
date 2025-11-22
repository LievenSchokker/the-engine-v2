#pragma once


#include "box2d/id.h"
#include "Physics/IPhysicsWorld.h"
#include "Physics/Components/DynamicBody.h"


class Box2DPhysicsWorld : public IPhysicsWorld
{
public:
	Box2DPhysicsWorld();
	~Box2DPhysicsWorld() override = default;
	void start() override;
	void update() override;
	void shutdown() override;
	b2BodyId addBody(const Transform* transform) override;

	b2WorldId GetWorldId() const;

private:
	b2WorldId worldId;
};