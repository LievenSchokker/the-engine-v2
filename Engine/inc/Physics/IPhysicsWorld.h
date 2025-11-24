#pragma once


struct b2BodyId;

#include "GameObject/GameObject.h"
#include "GameObject/GameObject.h"
#include "GameObject/Vector2.h"


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
	/**
	 * @brief Virtual destructor.
	 */
	virtual ~IPhysicsWorld() = default;

	/**
	 * @brief Initializes the physics world.
	 *
	 * Must be called before update() or creating any bodies.
	 * Implementations should set up gravity, world bounds, and other necessary parameters.
	 */
	virtual void start() = 0;

	/**
	 * @brief Steps the physics simulation forward.
	 *
	 * The concrete implementation decides the timestep size (fixed, variable, etc.)
	 * and should update all physics bodies accordingly.
	 */
	virtual void update() = 0;

	/**
	 * @brief Cleans up and shuts down the physics world.
	 *
	 * Implementations may free resources, destroy bodies, or reset world state.
	 * After this call, the world may be restarted with start().
	 */
	virtual void shutdown() = 0;

	/**
	 * @brief Creates a physics body for the given GameObject.
	 *
	 * Implementations typically inspect the GameObject’s Collider and RigidBody
	 * components to determine shape, mass, and body type.
	 *
	 * @param gameObject Pointer to a GameObject with Collider + RigidBody components.
	 */
	virtual void createBody(const GameObject* gameObject) = 0;

	/**
	 * @brief Destroys a previously created physics body.
	 *
	 * Removes the body from the physics simulation and cleans up any associated resources.
	 *
	 * @param body Pointer to the GameObject whose physics body should be destroyed.
	 */
	virtual void destroyBody(const GameObject* body) = 0;

	/**
	 * @brief Applies a force to the center of mass of a physics body.
	 *
	 * @param body Pointer to the GameObject whose body will receive the force.
	 * @param force Force vector in world units.
	 */
	virtual void applyForce(const GameObject* body, Vector2 force) = 0;

	/**
	 * @brief Synchronizes all registered GameObject transforms with the physics world.
	 *
	 * After stepping the simulation, this function updates each GameObject's
	 * Transform component to match the corresponding physics body's position and rotation.
	 */
	virtual void syncTransforms() = 0;
};