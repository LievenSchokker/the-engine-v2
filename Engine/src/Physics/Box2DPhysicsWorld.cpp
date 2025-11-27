#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"

Box2DPhysicsWorld::Box2DPhysicsWorld(float newTickRate)
	: tickRate(newTickRate)
{
}


void Box2DPhysicsWorld::start()
{
	worldId = {};
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {0.0f, 30.0f};

	b2WorldId box2dWorldId = b2CreateWorld(&worldDef);
	worldId = box2dWorldId;
}


void Box2DPhysicsWorld::update()
{
	float timeStep = 1.0f / tickRate;
	int subStepCount = 4;

	b2World_Step(worldId, timeStep, subStepCount);
}


void Box2DPhysicsWorld::createBody(const GameObject* gameObject)
{
	if (!gameObject) return;

	const Transform* transform = gameObject->getTransform();
	const Collider* collider = gameObject->getComponent<Collider>();
	const RigidBody* rigidBody = gameObject->getComponent<RigidBody>();
	if (!collider || !rigidBody) return;

	// Body definition
	b2BodyDef def = b2DefaultBodyDef();
	def.type = rigidBody->isDynamic ? b2_dynamicBody : b2_staticBody;
	def.position = {transform->getPosition().x, transform->getPosition().y};

	b2BodyId body = b2CreateBody(worldId, &def);

	// Shape/fixture
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = collider->density;
	shapeDef.isSensor = collider->isSensor;

	switch (collider->shape) {
		case PhysicsShapeType::Circle: {
			b2Circle circle{};
			circle.radius = collider->radius;
			b2CreateCircleShape(body, &shapeDef, &circle);
			break;
		}
		case PhysicsShapeType::Rectangle: {
			b2Polygon poly = b2MakeBox(collider->size.x * 0.5f,
			                           collider->size.y * 0.5f);
			b2CreatePolygonShape(body, &shapeDef, &poly);
			break;
		}
		default:
			break;
	}

	bodies[gameObject] = body;
}


void Box2DPhysicsWorld::destroyBody(const GameObject* gameObject)
{
	if (!gameObject) return;

	auto it = bodies.find(gameObject);
	if (it == bodies.end()) return;

	b2DestroyBody(it->second);
	bodies.erase(it);
}


void Box2DPhysicsWorld::applyForce(const GameObject* gameObject, Vector2 force)
{
	if (!gameObject) return;

	auto it = bodies.find(gameObject);
	if (it == bodies.end()) return;

	b2Vec2 b2Force = {force.x, force.y};
	b2Body_ApplyForceToCenter(it->second, b2Force, true);
}


void Box2DPhysicsWorld::syncTransforms()
{
	for (auto& [gameObject, box2DID] : bodies) {
		if (!gameObject) continue;

		b2Vec2 pos = b2Body_GetPosition(box2DID);
		b2Rot rot = b2Body_GetRotation(box2DID);

		gameObject->getTransform()->setPosition({ pos.x, pos.y });
		gameObject->getTransform()->setRotationAngle(b2Rot_GetAngle(rot));
	}
}


void Box2DPhysicsWorld::shutdown()
{
	for (auto& [gameObject, box2DID] : bodies) {
		b2DestroyBody(box2DID);
	}
	bodies.clear();
	worldId = b2_nullWorldId;
}