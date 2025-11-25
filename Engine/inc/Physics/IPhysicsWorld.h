#pragma once


#include "GameObject/GameObject.h"
#include "GameObject/Vector2.h"
#include "box2d/id.h"


/**
 * @interface IPhysicsWorld
 * @brief Abstract interface for a physics world implementation.
 *
 * This interface defines all operations required by the PhysicsSystem to interact
 * with an underlying physics engine (e.g., Box2D).
 */
class IPhysicsWorld
{
public:
	virtual ~IPhysicsWorld() = default;


	/**
	 * @brief Initializes the physics world.
	 *
	 * Must be called before calling update() or creating any bodies.
	 */
	virtual void start() = 0;


	/**
	 * @brief Steps the physics simulation forward.
	 *
	 * The concrete implementation decides on timestep size (fixed, variable, etc.).
	 */
	virtual void update() = 0;


	/**
	 * @brief Cleans up and shuts down the physics world.
	 *
	 * Implementations may free resources or reset world state.
	 */
	virtual void shutdown() = 0;


	/**
	 * @brief Creates a physics body for the given GameObject.
	 *
	 * Implementations typically inspect the GameObject’s Collider and RigidBody
	 * components to determine shape, mass, and body type.
	 *
	 * @param gameObject A GameObject with Collider + RigidBody components.
	 * @return A valid physics body ID or b2_nullBodyId on failure.
	 */
	virtual b2BodyId createBody(const GameObject* gameObject) = 0;


	/**
	 * @brief Destroys a previously created Box2D body.
	 *
	 * @param body The body ID returned from createBody().
	 */
	virtual void destroyBody(b2BodyId body) = 0;


	/**
	 * @brief Applies a force to the center of mass of a Box2D body.
	 *
	 * @param body The body to which the force should be applied.
	 * @param force The force vector, in world units.
	 */
	virtual void applyForce(b2BodyId body, Vector2 force) = 0;
};