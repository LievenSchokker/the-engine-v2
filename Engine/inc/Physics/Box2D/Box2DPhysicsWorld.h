#pragma once

#include "box2d/id.h"
#include "Physics/IPhysicsWorld.h"

/**
 * @class Box2DPhysicsWorld
 * @brief Low-level implementation of the physics world using Box2D.
 *
 * This class wraps the Box2D C API and provides concrete implementations for
 * the IPhysicsWorld interface. It handles world creation, stepping the physics
 * simulation, and creating/destroying physics bodies associated with GameObjects.
 *
 * The PhysicsSystem owns a single instance of this class and delegates all
 * physics operations to it.
 */
class Box2DPhysicsWorld : public IPhysicsWorld
{
public:
	/**
	 * @brief Constructs an empty physics world.
	 *
	 * The actual Box2D world is not created until start() is called.
	 */
	Box2DPhysicsWorld();
	~Box2DPhysicsWorld() override = default;

	/**
	 * @brief Initializes the Box2D world.
	 *
	 * This creates the internal b2WorldId instance and applies default
	 * world settings such as gravity. Must be called before update() or
	 * body creation.
	 */
	void start() override;

	/**
	 * @brief Steps the Box2D simulation forward by one fixed timestep.
	 *
	 * The timestep and substep count are typically configured internally.
	 * This method is called by PhysicsSystem::update().
	 */
	void update() override;

	/**
	 * @brief Shuts down the physics world.
	 *
	 * Currently a no-op, but included for interface completeness and future
	 * extensibility. All bodies should be destroyed before shutdown.
	 */
	void shutdown() override;

	/**
	 * @brief Creates a Box2D body from the components of a GameObject.
	 *
	 * The method inspects the GameObject's Collider and RigidBody components
	 * to configure and create the appropriate body in the Box2D world.
	 *
	 * @param gameObject The GameObject containing physics components.
	 * @return The created Box2D body ID, or b2_nullBodyId on failure.
	 */
	b2BodyId createBody(const GameObject* gameObject) override;

	/**
	 * @brief Destroys a previously created Box2D body.
	 *
	 * @param body The body ID returned from createBody().
	 */
	void destroyBody(b2BodyId body) override;

	/**
	 * @brief Applies a force to the center of mass of a Box2D body.
	 *
	 * @param body The body to which the force should be applied.
	 * @param force The force vector, in world units.
	 */
	void applyForce(b2BodyId body, Vector2 force) override;

private:
	/**
	 * @brief The internal Box2D world instance identifier.
	 *
	 * This value is created in start() and used for all subsequent
	 * Box2D operations such as stepping the simulation or creating bodies.
	 */
	b2WorldId worldId;
};
