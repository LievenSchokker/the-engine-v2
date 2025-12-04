#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"

Box2DPhysicsWorld::Box2DPhysicsWorld(float newTickRate)
    : worldId{}, tickRate(newTickRate)
{
}


void Box2DPhysicsWorld::start()
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 30.0f};

    b2WorldId box2dWorldId = b2CreateWorld(&worldDef);
    worldId = box2dWorldId;
}


void Box2DPhysicsWorld::fixedUpdate()
{
    float timeStep = 1.0f / tickRate;
    int subStepCount = 4;

    b2World_Step(worldId, timeStep, subStepCount);

    for (auto body : bodies) {
        RigidBody* rigid_body = const_cast<RigidBody*>(body.first);
        rigid_body->fixedUpdate();
    }

    syncTransforms();
}


void Box2DPhysicsWorld::createBody(const RigidBody* rigidBody)
{
    if (!rigidBody) return;

    // RigidBody always knows its owning GameObject
    GameObject* gameObject = rigidBody->getGameObject();
    if (!gameObject) return;

    auto* transform = gameObject->getTransform();
    auto* collider = gameObject->getComponent<Collider>();

    // Cannot create a physics body without a collider
    if (!collider || !transform) return;

    //  Body definition
    b2BodyDef def = b2DefaultBodyDef();
    def.type = rigidBody->isDynamic ? b2_dynamicBody : b2_staticBody;

    Vector2 pos = transform->getPosition();
    def.position = {pos.x(), pos.y()};

    b2BodyId body = b2CreateBody(worldId, &def);

	// Shape/fixture
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = collider->density;
    shapeDef.isSensor = collider->isSensor;

    // Create shape
    if (collider->shape == PhysicsShapeType::Circle) {
        b2Circle circle;
        circle.center = {0.0f, 0.0f};
        circle.radius = collider->radius;
        b2CreateCircleShape(body, &shapeDef, &circle);
    } else if (collider->shape == PhysicsShapeType::Rectangle) {
        b2Polygon poly = b2MakeBox(
            collider->size.x() * 0.5f,
            collider->size.y() * 0.5f
            );
        b2CreatePolygonShape(body, &shapeDef, &poly);
    }

    bodies[rigidBody] = body;
}


void Box2DPhysicsWorld::destroyBody(const RigidBody* rigidBody)
{
    if (!rigidBody) return;

    auto it = bodies.find(rigidBody);
    if (it == bodies.end()) return;

    b2DestroyBody(it->second);
    bodies.erase(it);
}


void Box2DPhysicsWorld::applyForce(const RigidBody* rigidBody, Vector2 force)
{
    if (!rigidBody) return;

    auto it = bodies.find(rigidBody);
    if (it == bodies.end()) return;

    b2Vec2 b2Force = {force.x(), force.y()};
    b2Body_ApplyForceToCenter(it->second, b2Force, true);
}


void Box2DPhysicsWorld::syncTransforms()
{
    for (auto& [rigidBody, box2DID] : bodies) {
        GameObject* gameObject = rigidBody->getGameObject();
        // component knows its owner

        b2Vec2 pos = b2Body_GetPosition(box2DID);
        b2Rot rot = b2Body_GetRotation(box2DID);

        gameObject->getTransform()->setPosition(Vector2(pos.x, pos.y));
        gameObject->getTransform()->setRotationAngle(b2Rot_GetAngle(rot));
    }
}


void Box2DPhysicsWorld::shutdown()
{
	for (auto& [gameObject, box2DID] : bodies) {
		b2DestroyBody(box2DID);
	}
	bodies.clear();
	b2DestroyWorld(worldId);
	worldId = b2_nullWorldId;
}
