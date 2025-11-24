#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"

Box2DPhysicsWorld::Box2DPhysicsWorld()
	: worldId(0)
{
}


void Box2DPhysicsWorld::start()
{
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {0.0f, 30.0f};

	b2WorldId box2dWorldId = b2CreateWorld(&worldDef);
	worldId = box2dWorldId;
}


void Box2DPhysicsWorld::update()
{
	float timeStep = 1.0f / 60.0f;
	int subStepCount = 4;

	b2World_Step(worldId, timeStep, subStepCount);
}


b2BodyId Box2DPhysicsWorld::createBody(const GameObject* gameObject)
{
	Transform* transform = gameObject->getTransform();
	Collider* collider = gameObject->getComponent<Collider>();
	RigidBody* rigidBody = gameObject->getComponent<RigidBody>();

	if (!collider || !rigidBody) {
		return b2_nullBodyId;
	}

	// -------- Create body --------
	b2BodyDef def = b2DefaultBodyDef();
	def.type = rigidBody->isDynamic ? b2_dynamicBody : b2_staticBody;

	def.position = {
		(float)transform->getPosition().x,
		(float)transform->getPosition().y
	};

	b2BodyId body = b2CreateBody(worldId, &def);


	// -------- Create shape/fixture --------
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = collider->density;
	shapeDef.isSensor = collider->isSensor;

	switch (collider->shape)
	{
		case PhysicsShapeType::Circle:
		{
			b2Circle circle = {0};
			circle.radius = collider->radius;
			b2CreateCircleShape(body, &shapeDef, &circle);
			break;
		}

		case PhysicsShapeType::Rectangle:
		{
			b2Polygon poly = b2MakeBox(
				collider->size.x * 0.5f,
				collider->size.y * 0.5f
			);

			b2CreatePolygonShape(body, &shapeDef, &poly);
			break;
		}

		default:
			break;
	}

	return body;
}

void Box2DPhysicsWorld::destroyBody(b2BodyId body)
{
	if (body.index1 != b2_nullBodyId.index1) {
		b2DestroyBody(body);
	}
}

void Box2DPhysicsWorld::shutdown()
{
}
