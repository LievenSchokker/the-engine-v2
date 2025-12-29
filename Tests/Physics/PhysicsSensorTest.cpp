#include "GameObject/GameObject.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"

#include <gtest/gtest.h>

class TestSensorBehaviour: public Behaviour
{
   public:
	int enterCount = 0;
	int exitCount = 0;
	Collider* lastOther = nullptr;

	void onSensorEnter(Collider* other) override
	{
		enterCount++;
		lastOther = other;
	}

	void onSensorExit(Collider* other) override
	{
		exitCount++;
		lastOther = other;
	}
};

// Verifies that calling onSensorEnter on a Collider
// invokes onSensorEnter on enabled behaviour
// attached to the same GameObject.
TEST(ColliderSensorTests, SensorEnterCallsEnabledBehaviours)
{
	GameObject sensorGO;
	GameObject otherGO;

	auto* sensorCollider = sensorGO.addComponent<Collider>();
	auto* otherCollider = otherGO.addComponent<Collider>();

	auto* behaviour = sensorGO.addComponent<TestSensorBehaviour>();

	sensorCollider->onSensorEnter(otherCollider);

	EXPECT_EQ(behaviour->enterCount, 1);
	EXPECT_EQ(behaviour->lastOther, otherCollider);
}

// Verifies that disabled behaviours do not receive
// sensor enter callbacks, even if the collider
// triggers a sensor enter event.
TEST(ColliderSensorTests, DisabledBehaviourDoesNotReceiveSensorEnter)
{
	GameObject sensorGO;
	GameObject otherGO;

	auto* sensorCollider = sensorGO.addComponent<Collider>();
	auto* otherCollider = otherGO.addComponent<Collider>();

	auto* behaviour = sensorGO.addComponent<TestSensorBehaviour>();
	behaviour->setEnabled(false);

	sensorCollider->onSensorEnter(otherCollider);

	EXPECT_EQ(behaviour->enterCount, 0);
}

// Verifies that multiple enabled behaviours attached
// to the same GameObject all receive the sensor
// enter callback when triggered.
TEST(ColliderSensorTests, MultipleBehavioursReceiveSensorEnter)
{
	GameObject sensorGO;
	GameObject otherGO;

	auto* sensorCollider = sensorGO.addComponent<Collider>();
	auto* otherCollider = otherGO.addComponent<Collider>();

	auto* b1 = sensorGO.addComponent<TestSensorBehaviour>();
	auto* b2 = sensorGO.addComponent<TestSensorBehaviour>();

	sensorCollider->onSensorEnter(otherCollider);

	EXPECT_EQ(b1->enterCount, 1);
	EXPECT_EQ(b2->enterCount, 1);
}

// Verifies that calling onSensorExit on a Collider
// correctly forwards the exit event to all enabled
// behaviours on the owning GameObject.
TEST(ColliderSensorTests, SensorExitCallsBehaviour)
{
	GameObject sensorGO;
	GameObject otherGO;

	auto* sensorCollider = sensorGO.addComponent<Collider>();
	auto* otherCollider = otherGO.addComponent<Collider>();

	auto* behaviour = sensorGO.addComponent<TestSensorBehaviour>();

	sensorCollider->onSensorExit(otherCollider);

	EXPECT_EQ(behaviour->exitCount, 1);
	EXPECT_EQ(behaviour->lastOther, otherCollider);
}

// Verifies that the sensor flag set on a Collider
// is stored correctly and can be queried later
// using the isSensor accessor.
TEST(ColliderTests, SensorFlagIsStoredCorrectly)
{
	Collider c;
	c.setSensor(true);

	EXPECT_TRUE(c.isSensor());
}

// Verifies that setting the density on a Collider
// correctly stores the value and returns the same
// value when queried.
TEST(ColliderTests, DensityIsStoredCorrectly)
{
	Collider c;
	c.setDensity(2.5f);

	EXPECT_FLOAT_EQ(c.getDensity(), 2.5f);
}

// Verifies that setting a circle shape on a Collider
// correctly updates the shape type and stores
// the provided radius value.
TEST(ColliderTests, ShapeIsSetCorrectly)
{
	Collider c;
	c.setCircle(10.0f);

	EXPECT_EQ(c.getShape(), PhysicsShapeType::Circle);
	EXPECT_FLOAT_EQ(c.radius, 10.0f);
}

// Verifies that creating a physics body with a valid
// GameObject, RigidBody, and Collider does not crash
// the physics world during body creation.
TEST(Box2DPhysicsWorldTests, CreateBodyDoesNotCrash)
{
	Box2DPhysicsWorld world;
	world.start();

	GameObject go;
	go.addComponent<Transform>();
	go.addComponent<RigidBody>();
	auto* col = go.addComponent<Collider>();
	col->setCircle(1.0f);

	EXPECT_NO_THROW(world.createBody(go.getComponent<RigidBody>()));
}
