#pragma once
#include "Core/IEngineSystems.h"

class RigidBody;
struct b2BodyId;

#include "Math/Vector2.h"


/**
 * @interface IPhysicsWorld
 * @brief Abstract interface for a physics world implementation.
 *
 * This interface defines all operations required by the PhysicsSystem to interact
 * with an underlying physics engine (e.g., Box2D, Bullet, or custom physics).
 *
 * Implementations of this interface are responsible for:
 *  - Creating and destroying physics bodies for GameObjects.
 *  - Stepping the physics simulation.
 *  - Applying forces to bodies.
 *  - Synchronizing GameObject transforms with the physics world.
 */
class IPhysicsWorld
{
public:
	virtual ~IPhysicsWorld() = default;
	/**
	 * @brief Creates a physics body for the given RigidBody.
	 *
	 * Implementations typically inspect the GameObject’s Collider and RigidBody
	 * components to determine shape, mass, and body type.
	 *
	 * @param rigidBody Pointer to a RigidBody with Collider + RigidBody components.
	 */
	virtual void createBody(const RigidBody* rigidBody) = 0;

	/**
	 * @brief Destroys a previously created physics body.
	 *
	 * Removes the body from the physics simulation and cleans up any associated resources.
	 *
	 * @param rigidBody Pointer to the RigidBody whose physics body should be destroyed.
	 */
	virtual void destroyBody(const RigidBody* rigidBody) = 0;

	/**
	 * @brief Applies a force to the center of mass of a physics body.
	 *
	 * @param rigidBody Pointer to the RigidBody whose body will receive the force.
	 * @param force Force vector in world units.
	 */
	virtual void applyForce(const RigidBody* rigidBody, Vector2 force) = 0;

	/**
	 * @brief Synchronizes all registered GameObject transforms with the physics world.
	 *
	 * After stepping the simulation, this function updates each GameObject's
	 * Transform component to match the corresponding physics body's position and rotation.
	 */
	virtual void syncTransforms() = 0;
	virtual void applyNetworkSnapshot() = 0;
	virtual void initialize() = 0;
	virtual void step(float deltaTime) = 0;
	virtual void shutdown() = 0;
};