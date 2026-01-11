#include "Physics/Box2D/Box2DPhysicsWorld.h"

#include "Component/Transform.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "box2d/box2d.h"
#include "box2d/types.h"

#include <iostream>

namespace
{
constexpr float kDegToRad = 3.14159265359f / 180.0f;
constexpr float kRadToDeg = 180.0f / 3.14159265359f;
}

Box2DPhysicsWorld::Box2DPhysicsWorld(float newTickRate)
	: worldId{}, tickRate(newTickRate)
{
}

void Box2DPhysicsWorld::initialize()
{
	std::cout << "Box2DPhysicsWorld::initialize() called" << std::endl;

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {0.0f, 150.0f};
	worldId = b2CreateWorld(&worldDef);

	std::cout << "Box2D world created, id.index1 = " << worldId.index1
			  << std::endl;
}

void Box2DPhysicsWorld::step(float deltaTime)
{
	const int subStepCount = 4;

	b2World_Step(worldId, deltaTime, subStepCount);
	handleEvents();
	syncTransforms();
}

void Box2DPhysicsWorld::handleEvents()
{
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
}

void Box2DPhysicsWorld::createBody(const RigidBody* rigidBody)
{
	if ( !rigidBody ) return;

	// RigidBody always knows its owning GameObject
	GameObject* gameObject = rigidBody->getGameObject();
	if ( !gameObject ) return;

	auto* transform = gameObject->getTransform();
	auto* collider = gameObject->getComponent<Collider>();

	// Cannot create a physics body without a collider
	if ( !collider || !transform ) return;

	//  Body definition
	b2BodyDef def = b2DefaultBodyDef();
	switch ( rigidBody->getBodyType() )
	{
		case RigidBody::BodyType::Static:
			def.type = b2_staticBody;
			break;
		case RigidBody::BodyType::Kinematic:
			def.type = b2_kinematicBody;
			break;
		case RigidBody::BodyType::Dynamic:
		default:
			def.type = b2_dynamicBody;
			break;
	}
	def.fixedRotation = rigidBody->isFixedRotation();
	def.linearDamping = rigidBody->getLinearDamping();
	def.gravityScale = rigidBody->getGravityScale();
	def.isBullet = rigidBody->getBullet();

	Vector2 pos = transform->getPosition();
	def.position = {pos.x, pos.y};
	def.rotation = b2MakeRot(transform->getRotationAngle() * kDegToRad);

	b2BodyId body = b2CreateBody(worldId, &def);

	// Shape definition
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = collider->getDensity();
	shapeDef.isSensor = collider->isSensor();
	shapeDef.enableSensorEvents = true;

	// Create the correct shape and get b2ShapeId
	b2ShapeId shapeId = b2_nullShapeId;

	if ( collider->getShape() == PhysicsShapeType::Circle )
	{
		b2Circle circle;
		circle.center = {0.0f, 0.0f};
		circle.radius = collider->radius;

		shapeId = b2CreateCircleShape(body, &shapeDef, &circle);
	}
	else if ( collider->getShape() == PhysicsShapeType::Rectangle )
	{
		b2Polygon poly =
			b2MakeBox(collider->size.x * 0.5f, collider->size.y * 0.5f);

		shapeId = b2CreatePolygonShape(body, &shapeDef, &poly);
	}

	if ( b2Shape_IsValid(shapeId) )
	{
		b2Shape_SetUserData(shapeId, gameObject);
		b2Shape_SetRestitution(shapeId, collider->getRestitution());
	}

	// Assign user data to the body (optional)
	b2Body_SetUserData(body, gameObject);

	// Store mapping RigidBody* → b2BodyId
	bodies[rigidBody] = body;
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

void Box2DPhysicsWorld::setGravity(Vector2 gravity)
{
	b2Vec2 gravityVec = {gravity.x, gravity.y};
	b2World_SetGravity(worldId, gravityVec);
}

void Box2DPhysicsWorld::setBodyTransform(const RigidBody* rigidBody,
										 Vector2 position,
										 float rotationAngle)
{
	if ( !rigidBody ) return;

	auto it = bodies.find(rigidBody);
	if ( it == bodies.end() ) return;

	b2Vec2 b2Position = {position.x, position.y};
	b2Rot b2Rotation = b2MakeRot(rotationAngle * kDegToRad);
	b2Body_SetTransform(it->second, b2Position, b2Rotation);
}

void Box2DPhysicsWorld::setLinearVelocity(const RigidBody* rigidBody,
										  Vector2 velocity)
{
	if ( !rigidBody ) return;

	auto it = bodies.find(rigidBody);
	if ( it == bodies.end() ) return;

	b2BodyId bodyId = it->second;

	b2Body_SetLinearVelocity(bodyId, {velocity.x, velocity.y});
	b2Body_SetAwake(bodyId, true);
}

Vector2 Box2DPhysicsWorld::getLinearVelocity(const RigidBody* rigidBody) const
{
	if ( !rigidBody ) return {0.0f, 0.0f};

	auto it = bodies.find(rigidBody);
	if ( it == bodies.end() ) return {0.0f, 0.0f};

	b2Vec2 vel = b2Body_GetLinearVelocity(it->second);
	return {vel.x, vel.y};
}

void Box2DPhysicsWorld::shutdown()
{
	for ( auto& [gameObject, box2DID] : bodies )
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

		b2Vec2 position = b2Body_GetPosition(box2DID);
		b2Rot rotation = b2Body_GetRotation(box2DID);
		b2Vec2 linearVel = b2Body_GetLinearVelocity(box2DID);
		float angularVel = b2Body_GetAngularVelocity(box2DID);

		gameObject->getTransform()->setPosition(
			Vector2(position.x, position.y));
		gameObject->getTransform()->setRotationAngle(
			b2Rot_GetAngle(rotation) * kRadToDeg);

		auto* rb = const_cast<RigidBody*>(rigidBody);
		rb->linearVelocity = {linearVel.x, linearVel.y};
		rb->angularVelocity = angularVel;
	}
}

void Box2DPhysicsWorld::applyNetworkSnapshot()
{
	constexpr float positionLerpFactor = 0.3f;
	constexpr float rotationLerpFactor = 0.3f;
	constexpr float snapThresholdSquared = 25.0f;
	constexpr float closeEnoughSquared = 0.01f;

	for ( auto& [rigidBody, bodyId] : bodies )
	{
		if ( !rigidBody->isDynamic ) continue;

		const GameObject* gameObject = rigidBody->getGameObject();
		if ( !gameObject ) continue;

		const Transform* transform = gameObject->getTransform();
		if ( !transform ) continue;

	const Vector2 targetPos = transform->getPosition();
	const float targetAngle = transform->getRotationAngle() * kDegToRad;

		b2Vec2 currentPos = b2Body_GetPosition(bodyId);
		b2Rot currentRot = b2Body_GetRotation(bodyId);
		float currentAngle = b2Rot_GetAngle(currentRot);

		float dx = targetPos.x - currentPos.x;
		float dy = targetPos.y - currentPos.y;
		float distanceSquared = dx * dx + dy * dy;

		Vector2 newPosition{0, 0};
		float newAngle;

		if ( distanceSquared > snapThresholdSquared )
		{
			newPosition = targetPos;
			newAngle = targetAngle;
		}
		else if ( distanceSquared < closeEnoughSquared )
		{
			newPosition = targetPos;
			newAngle = targetAngle;
		}
		else
		{
			newPosition.x = currentPos.x + dx * positionLerpFactor;
			newPosition.y = currentPos.y + dy * positionLerpFactor;

			float angleDiff = targetAngle - currentAngle;
			while ( angleDiff > 3.14159265359f )
				angleDiff -= 2.0f * 3.14159265359f;
			while ( angleDiff < -3.14159265359f )
				angleDiff += 2.0f * 3.14159265359f;
			newAngle = currentAngle + angleDiff * rotationLerpFactor;
		}

		b2Body_SetTransform(bodyId, {newPosition.x, newPosition.y},
							b2MakeRot(newAngle));
		b2Body_SetLinearVelocity(
			bodyId, {rigidBody->linearVelocity.x, rigidBody->linearVelocity.y});
		b2Body_SetAngularVelocity(bodyId, rigidBody->angularVelocity);
		b2Body_SetAwake(bodyId, true);
	}
}
