#include "Physics/Box2D/Box2DPhysicsWorld.h"

#include "Component/Transform.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "box2d/box2d.h"
#include "box2d/types.h"

#include <iostream>

#include <iostream>

Box2DPhysicsWorld::Box2DPhysicsWorld(float newTickRate)
	: worldId{}
	  , tickRate(newTickRate)
{
}

void Box2DPhysicsWorld::initialize()
{
	std::cout << "Box2DPhysicsWorld::initialize() called" << std::endl;

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {0.0f, 30.0f};
	worldId = b2CreateWorld(&worldDef);

	std::cout << "Box2D world created, id.index1 = " << worldId.index1 << std::endl;
}

void Box2DPhysicsWorld::step(float deltaTime)
{
	float timeStep = 1.0f / tickRate;
	int subStepCount = 4;

	b2World_Step(worldId, timeStep, subStepCount);
	b2SensorEvents sensorEvents = b2World_GetSensorEvents(worldId);

	// Look at the sensor touches
	for ( int i = 0; i < sensorEvents.beginCount; ++i )
	{
		const b2SensorBeginTouchEvent& event = sensorEvents.beginEvents[i];

		if ( !b2Shape_IsValid(event.sensorShapeId) ||
			 !b2Shape_IsValid(event.visitorShapeId) )
			continue;

		void* sensorUser = b2Shape_GetUserData(event.sensorShapeId);
		void* visitorUser = b2Shape_GetUserData(event.visitorShapeId);

		GameObject* sensorGO = static_cast<GameObject*>(sensorUser);
		GameObject* visitorGO = static_cast<GameObject*>(visitorUser);

		if ( !sensorGO || !visitorGO ) continue;


		Collider* sensorCol = sensorGO->getComponent<Collider>();
		Collider* visitorCol = visitorGO->getComponent<Collider>();

		// TODO add event with new event system
		if ( sensorCol && visitorCol ) sensorCol->onSensorEnter(visitorCol);
	}

	// call end sensor exit events
	for ( int i = 0; i < sensorEvents.endCount; ++i )
	{
		const b2SensorEndTouchEvent& event = sensorEvents.endEvents[i];

		if ( !b2Shape_IsValid(event.sensorShapeId) ||
			 !b2Shape_IsValid(event.visitorShapeId) )
			continue;

		void* sensorUser = b2Shape_GetUserData(event.sensorShapeId);
		void* visitorUser = b2Shape_GetUserData(event.visitorShapeId);

		GameObject* sensorGO = static_cast<GameObject*>(sensorUser);
		GameObject* visitorGO = static_cast<GameObject*>(visitorUser);

		if ( !sensorGO || !visitorGO ) continue;

		Collider* sensorCol = sensorGO->getComponent<Collider>();
		Collider* visitorCol = visitorGO->getComponent<Collider>();

		// TODO add event with new event system
		if ( sensorCol && visitorCol ) sensorCol->onSensorExit(visitorCol);
	}

	// Sync transforms to gameobjects

	for (auto body : bodies)
	{
		RigidBody* rigid_body = const_cast<RigidBody*>(body.first);
		rigid_body->fixedUpdate();
	}

	syncTransforms();
}

void Box2DPhysicsWorld::createBody(const RigidBody* rigidBody)
{
    std::cout << "[createBody] Called" << std::endl;

    if (!rigidBody) {
        std::cout << "[createBody] FAIL: rigidBody is null" << std::endl;
        return;
    }

    GameObject* gameObject = rigidBody->getGameObject();
    if (!gameObject) {
        std::cout << "[createBody] FAIL: gameObject is null" << std::endl;
        return;
    }
    std::cout << "[createBody] gameObject: " << gameObject->getName() << std::endl;

    auto* transform = gameObject->getTransform();
    auto* collider = gameObject->getComponent<Collider>();

    if (!collider || !transform) {
        std::cout << "[createBody] FAIL: missing collider or transform" << std::endl;
        return;
    }

    // THIS IS THE KEY LINE - check what shape is set
    std::cout << "[createBody] Collider shape: " << (int)collider->getShape() << std::endl;
    std::cout << "[createBody] isDynamic: " << rigidBody->isDynamic << std::endl;

    b2BodyDef def = b2DefaultBodyDef();
    def.type = rigidBody->isDynamic ? b2_dynamicBody : b2_staticBody;

    Vector2 pos = transform->getPosition();
    def.position = {pos.x, pos.y};
    std::cout << "[createBody] Position: " << pos.x << ", " << pos.y << std::endl;

    def.rotation = b2MakeRot(transform->getRotationAngle());

    b2BodyId body = b2CreateBody(worldId, &def);
    std::cout << "[createBody] Body created with b2 id" << std::endl;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = collider->getDensity();
    shapeDef.isSensor = collider->isSensor();
    shapeDef.enableSensorEvents = true;

    b2ShapeId shapeId = b2_nullShapeId;

    if (collider->getShape() == PhysicsShapeType::Circle)
    {
        std::cout << "[createBody] Creating circle shape, radius: " << collider->radius << std::endl;
        b2Circle circle;
        circle.center = {0.0f, 0.0f};
        circle.radius = collider->radius;
        shapeId = b2CreateCircleShape(body, &shapeDef, &circle);
    }
    else if (collider->getShape() == PhysicsShapeType::Rectangle)
    {
        std::cout << "[createBody] Creating rectangle shape, size: " << collider->size.x << ", " << collider->size.y << std::endl;
        b2Polygon poly = b2MakeBox(collider->size.x * 0.5f, collider->size.y * 0.5f);
        shapeId = b2CreatePolygonShape(body, &shapeDef, &poly);
    }
    else
    {
        std::cout << "[createBody] WARNING: Unknown shape type!" << std::endl;
    }

    if (b2Shape_IsValid(shapeId))
    {
        b2Shape_SetUserData(shapeId, gameObject);
        std::cout << "[createBody] Shape created successfully" << std::endl;
    }
    else
    {
        std::cout << "[createBody] WARNING: Shape creation failed!" << std::endl;
    }

    b2Body_SetUserData(body, gameObject);
    bodies[rigidBody] = body;

    std::cout << "[createBody] SUCCESS! Total bodies: " << bodies.size() << std::endl;
}

void Box2DPhysicsWorld::destroyBody(const RigidBody* rigidBody)
{
	if ( !rigidBody ) return;

	auto it = bodies.find(rigidBody);
	if ( it == bodies.end() ) return;

	b2DestroyBody(it->second);
	bodies.erase(it);
}

void Box2DPhysicsWorld::applyForce(const RigidBody* rigidBody, Vector2 force)
{
	if ( !rigidBody ) return;

	auto it = bodies.find(rigidBody);
	if ( it == bodies.end() ) return;

	b2Vec2 b2Force = {force.x, force.y};
	b2Body_ApplyForceToCenter(it->second, b2Force, true);
}


void Box2DPhysicsWorld::shutdown()
{
	for (auto& [gameObject, box2DID] : bodies)
	{
		b2DestroyBody(box2DID);
	}
	bodies.clear();
	b2DestroyWorld(worldId);
	worldId = b2_nullWorldId;
}

void Box2DPhysicsWorld::syncTransforms()
{
	for ( auto& [rigidBody, box2DID] : bodies )
	{
		GameObject* gameObject = rigidBody->getGameObject();

		// component knows its owner
		b2Vec2 pos = b2Body_GetPosition(box2DID);
		b2Rot rot = b2Body_GetRotation(box2DID);

		gameObject->getTransform()->setPosition(Vector2(pos.x, pos.y));
		gameObject->getTransform()->setRotationAngle(b2Rot_GetAngle(rot));
	}
}

void Box2DPhysicsWorld::applyNetworkSnapshot()
{
	for (auto& [rigidBody, bodyId] : bodies)
	{
		GameObject* gameObject = rigidBody->getGameObject();
		if (!gameObject) continue;

		Transform* transform = gameObject->getTransform();
		if (!transform) continue;

		Vector2 pos = transform->getPosition();
		float angle = transform->getRotationAngle();
		b2Body_SetTransform(bodyId, {pos.x, pos.y}, b2MakeRot(angle));
		b2Body_SetAwake(bodyId, true);
	}
}