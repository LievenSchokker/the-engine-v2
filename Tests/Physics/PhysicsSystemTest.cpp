//
// Created by thijs on 25-11-2025.
//


#include <gtest/gtest.h>
#include "Physics/IPhysicsWorld.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/Components/Collider.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"


/**
 * @brief Tests that applying a force updates the GameObject's transform.
 */
TEST(Box2DPhysicsWorldTest, PhysicsSimulationUpdatesTransform)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject obj;
    obj.addComponent<Transform>();
    obj.addComponent<Collider>()->setCircle(10);
    obj.addComponent<RigidBody>();

    world.createBody(obj.getComponent<RigidBody>());

    Vector2 initialPos = obj.getTransform()->getPosition();

    world.applyForce(obj.getComponent<RigidBody>(), {0, 100});
    world.fixedUpdate();
    world.syncTransforms();

    Vector2 newPos = obj.getTransform()->getPosition();
    EXPECT_NE(initialPos.y, newPos.y);
}


/**
 * @brief Objects fall under gravity over multiple frames.
 */

TEST(Box2DPhysicsWorldTest, ObjectFallsUnderGravity)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject obj;
    obj.addComponent<Transform>();
    obj.addComponent<Collider>()->setRectangle({10, 10});
    obj.addComponent<RigidBody>();

    world.createBody(obj.getComponent<RigidBody>());

    Vector2 initialPos = obj.getTransform()->getPosition();

    for (int i = 0; i < 60; ++i) {
        world.fixedUpdate();
        world.syncTransforms();
    }

    Vector2 newPos = obj.getTransform()->getPosition();

    // Box2D default gravity pushes DOWN in +Y direction
    EXPECT_GT(newPos.y, initialPos.y);
}


/**
 * @brief Static objects should not move under forces.
 */
TEST(Box2DPhysicsWorldTest, StaticObjectDoesNotMove)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject obj;
    obj.addComponent<Transform>();
    obj.addComponent<Collider>()->setRectangle({10, 10});
    auto rb = obj.addComponent<RigidBody>();
    rb->makeStatic();

    world.createBody(obj.getComponent<RigidBody>());

    Vector2 initialPos = obj.getTransform()->getPosition();

    world.applyForce(obj.getComponent<RigidBody>(), {0, 100});
    world.fixedUpdate();
    world.syncTransforms();

    Vector2 newPos = obj.getTransform()->getPosition();
    EXPECT_EQ(initialPos.y, newPos.y);
}


/**
 * @brief Applying horizontal force moves object in X axis.
 */
TEST(Box2DPhysicsWorldTest, ApplyHorizontalForceMovesObject)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject obj;
    obj.addComponent<Transform>();
    obj.addComponent<Collider>()->setRectangle({10, 10});
    obj.addComponent<RigidBody>();

    world.createBody(obj.getComponent<RigidBody>());

    Vector2 initialPos = obj.getTransform()->getPosition();

    world.applyForce(obj.getComponent<RigidBody>(), {50, 0});
    world.fixedUpdate();
    world.syncTransforms();

    Vector2 newPos = obj.getTransform()->getPosition();
    EXPECT_GT(newPos.x, initialPos.x); // Object moved horizontally
}


/**
 * @brief Tests collision between two objects (ball falls on floor).
 */
TEST(Box2DPhysicsWorldTest, ObjectsCollide)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject floor;
    floor.addComponent<Transform>();
    floor.addComponent<Collider>()->setRectangle({200, 20});
    floor.addComponent<RigidBody>()->makeStatic();

    GameObject ball;
    ball.addComponent<Transform>();
    ball.addComponent<Collider>()->setCircle(10);
    ball.addComponent<RigidBody>();

    world.createBody(floor.getComponent<RigidBody>());
    world.createBody(ball.getComponent<RigidBody>());

    for (int i = 0; i < 120; ++i) {
        world.fixedUpdate();
        world.syncTransforms();
    }

    Vector2 ballPos = ball.getTransform()->getPosition();
    EXPECT_LE(ballPos.y, 100 - 10); // Ball rests on floor (radius 10)
}


/**
 * @brief Tests multiple forces applied in one frame.
 */
TEST(Box2DPhysicsWorldTest, ApplyMultipleForces)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject obj;
    obj.addComponent<Transform>();
    obj.addComponent<Collider>()->setRectangle({10, 10});
    obj.addComponent<RigidBody>();

    world.createBody(obj.getComponent<RigidBody>());

    world.applyForce(obj.getComponent<RigidBody>(), {50, 0});
    world.applyForce(obj.getComponent<RigidBody>(), {0, 100});

    world.fixedUpdate();
    world.syncTransforms();

    Vector2 pos = obj.getTransform()->getPosition();
    EXPECT_GT(pos.x, 0);
    EXPECT_GT(pos.y, 0);
}


/**
 * @brief Tests that destroying a static object removes it from simulation.
 */
TEST(Box2DPhysicsWorldTest, DestroyStaticObject)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject obj;
    obj.addComponent<Transform>();
    obj.addComponent<Collider>()->setRectangle({10, 10});
    obj.addComponent<RigidBody>()->makeStatic();

    world.createBody(obj.getComponent<RigidBody>());

    Vector2 initialPos = obj.getTransform()->getPosition();

    // Destroy the body
    world.destroyBody(obj.getComponent<RigidBody>());

    // Apply a force and update world
    world.applyForce(obj.getComponent<RigidBody>(), {0, 100});
    world.fixedUpdate();
    world.syncTransforms();

    Vector2 newPos = obj.getTransform()->getPosition();
    EXPECT_EQ(newPos.y, initialPos.y); // Object no longer simulated
}


/**
 * @brief Tests that destroying a dynamic object stops it from moving.
 */
TEST(Box2DPhysicsWorldTest, DestroyDynamicObject)
{
    Box2DPhysicsWorld world;
    world.start();

    GameObject obj;
    obj.addComponent<Transform>();
    obj.addComponent<Collider>()->setCircle(10);
    obj.addComponent<RigidBody>();

    world.createBody(obj.getComponent<RigidBody>());

    Vector2 initialPos = obj.getTransform()->getPosition();

    // Destroy the body immediately
    world.destroyBody(obj.getComponent<RigidBody>());

    // Apply a force and simulate multiple frames
    for (int i = 0; i < 10; ++i) {
        world.applyForce(obj.getComponent<RigidBody>(), {0, 100});
        world.fixedUpdate();
        world.syncTransforms();
    }

	Vector2 newPos = obj.getTransform()->getPosition();
	EXPECT_EQ(newPos.x, initialPos.x);
	EXPECT_EQ(newPos.y, initialPos.y); // No movement after destruction
}
