
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "Component/Transform.h"

Box2DPhysicsWorld::Box2DPhysicsWorld()
	: worldId(0)
{

}


void Box2DPhysicsWorld::start()
{
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {0.0f, 10.0f};
	b2WorldId box2dWorldId = b2CreateWorld(&worldDef);
	worldId = box2dWorldId;

	b2BodyDef groundBodyDef = b2DefaultBodyDef();
	groundBodyDef.position = {0.0f, -10.0f};

	b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
	b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
}

void Box2DPhysicsWorld::update()
{
	float timeStep = 1.0f / 60.0f;
	int subStepCount = 4;

	b2World_Step(worldId, timeStep, subStepCount);
}

b2BodyId Box2DPhysicsWorld::addBody(const Transform* transform)
{
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = {static_cast<float>(transform->getPosition().x), static_cast<float>(transform->getPosition().y)};
	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
	return bodyId;
}

void Box2DPhysicsWorld::shutdown()
{

}

b2WorldId Box2DPhysicsWorld::GetWorldId() const
{
	return worldId;
}
