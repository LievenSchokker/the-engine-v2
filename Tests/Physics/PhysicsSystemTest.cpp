//
// Created by thijs on 25-11-2025.
//


#include <gtest/gtest.h>
#include "Physics/PhysicsSystem.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/Components/Collider.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"


// Start the physics world
TEST(PhysicsSystem, PhysicsSimulationUpdatesTransform)
{
	PhysicsSystem system;
	system.start();

	GameObject obj;
	obj.addComponent<Transform>();
	obj.addComponent<Collider>()->setCircle(10);
	obj.addComponent<RigidBody>();

	system.registerBody(&obj);

	Vector2 initialPos = obj.getTransform()->getPosition();

	system.applyForce(&obj, {0, 100});
	system.update(1.0f / 60.0f);
	system.syncData();

	Vector2 newPos = obj.getTransform()->getPosition();
	EXPECT_NE(initialPos.y, newPos.y);  // The object moved
}

// Objects fall under gravity
TEST(PhysicsSystem, ObjectFallsUnderGravity)
{
	PhysicsSystem system;
	system.start();

	GameObject obj;
	obj.addComponent<Transform>();
	obj.addComponent<Collider>()->setRectangle({10, 10});
	obj.addComponent<RigidBody>();

	system.registerBody(&obj);

	Vector2 initialPos = obj.getTransform()->getPosition();

	// Simulate multiple frames
	for (int i = 0; i < 60; ++i) {
		system.update(1.0f / 60.0f);
		system.syncData();
	}

	Vector2 newPos = obj.getTransform()->getPosition();
	EXPECT_GT(newPos.y, initialPos.y);  // The object fell down
}

// static objects do not move
TEST(PhysicsSystem, StaticObjectDoesNotMove)
{
	PhysicsSystem system;
	system.start();

	GameObject obj;
	obj.addComponent<Transform>();
	obj.addComponent<Collider>()->setRectangle({10, 10});
	auto rb = obj.addComponent<RigidBody>();
	rb->makeStatic();

	system.registerBody(&obj);

	Vector2 initialPos = obj.getTransform()->getPosition();

	system.applyForce(&obj, {0, 100});
	system.update(1.0f / 60.0f);
	system.syncData();

	Vector2 newPos = obj.getTransform()->getPosition();
	EXPECT_EQ(newPos.y, initialPos.y);  // Static object did not move
}

// Applying a horizontal force moves object in X
TEST(PhysicsSystem, ApplyHorizontalForceMovesObject)
{
	PhysicsSystem system;
	system.start();

	GameObject obj;
	obj.addComponent<Transform>();
	obj.addComponent<Collider>()->setRectangle({10, 10});
	obj.addComponent<RigidBody>();

	system.registerBody(&obj);

	Vector2 initialPos = obj.getTransform()->getPosition();

	system.applyForce(&obj, {50, 0});
	system.update(1.0f / 60.0f);
	system.syncData();

	Vector2 newPos = obj.getTransform()->getPosition();
	EXPECT_GT(newPos.x, initialPos.x);  // Object moved horizontally
}

// Two object collide
TEST(PhysicsSystem, ObjectsCollide)
{
	PhysicsSystem system;
	system.start();

	GameObject floor;
	floor.addComponent<Transform>();
	floor.addComponent<Collider>()->setRectangle({200, 20});
	floor.addComponent<RigidBody>()->makeStatic();

	GameObject ball;
	ball.addComponent<Transform>();
	ball.addComponent<Collider>()->setCircle(10);
	ball.addComponent<RigidBody>();

	system.registerBody(&floor);
	system.registerBody(&ball);

	// Simulate multiple frames
	for (int i = 0; i < 120; ++i) {
		system.update(1.0f / 60.0f);
		system.syncData();
	}

	Vector2 ballPos = ball.getTransform()->getPosition();
	EXPECT_LE(ballPos.y, 100 - 10);  // Ball is resting on top of the floor (radius 10)
}


// Multiple forces applied
TEST(PhysicsSystem, ApplyMultipleForces)
{
	PhysicsSystem system;
	system.start();

	GameObject obj;
	obj.addComponent<Transform>();
	obj.addComponent<Collider>()->setRectangle({10, 10});
	obj.addComponent<RigidBody>();

	system.registerBody(&obj);

	system.applyForce(&obj, {50, 0});
	system.applyForce(&obj, {0, 100});

	system.update(1.0f / 60.0f);
	system.syncData();

	Vector2 pos = obj.getTransform()->getPosition();
	EXPECT_GT(pos.x, 0);
	EXPECT_GT(pos.y, 0);
}
