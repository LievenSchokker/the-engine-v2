#include "Physics/PhysicsSystem.h"

#include "Component/Transform.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include <box2d/box2d.h>
#include <box2d/math_functions.h>


PhysicsSystem::PhysicsSystem()
	: world(std::make_unique<Box2DPhysicsWorld>())
{
}


void PhysicsSystem::start() const
{
	world->start();
}

// TODO delta time is not used -- https://stackoverflow.com/questions/5466432/how-do-i-implement-better-time-step-fixed-or-semi-fixed-in-box2d
void PhysicsSystem::update(float dt) const
{
	world->update();
}


void PhysicsSystem::registerBody(const GameObject* gameObject)
{
	if (!gameObject) return;

	b2BodyId bodyId = world->createBody(gameObject);
	if (bodyId.index1 != b2_nullBodyId.index1) {
		physicObjects[gameObject] = bodyId;
	}
}


void PhysicsSystem::unregisterBody(const GameObject* gameObject)
{
	if (!gameObject) return;

	auto it = physicObjects.find(gameObject);
	if (it != physicObjects.end()) {
		b2BodyId bodyId = it->second;
		world->destroyBody(bodyId);

		physicObjects.erase(it);
	}
}


void PhysicsSystem::syncData()
{
	for (auto& [gameObject, bodyId] : physicObjects) {
		if (!gameObject) continue;

		// Get position and rotation from Box2D
		b2Vec2 pos = b2Body_GetPosition(bodyId);
		b2Rot rot = b2Body_GetRotation(bodyId);

		// Update the GameObject transform
		Transform* transform = gameObject->getTransform();
		transform->setPosition({pos.x, pos.y});
		transform->setRotationAngle(b2Rot_GetAngle(rot));
	}
}


void PhysicsSystem::applyForce(const GameObject* gameObject, Vector2 force)
{
	if (!gameObject) return;

	auto it = physicObjects.find(gameObject);
	if (it == physicObjects.end()) return;

	b2BodyId bodyId = it->second;
	world->applyForce(bodyId, force);
}