#pragma once

#include "box2d/id.h"
#include "Physics/IPhysicsWorld.h"
#include "Physics/Components/RigidBody.h"

#include <unordered_map>

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
class Box2DPhysicsWorld: public IPhysicsWorld
{
public:
	/**
	 * @brief Constructs an empty physics world.
	 *
	 * The actual Box2D world is not created until start() is called.
	 */
	Box2DPhysicsWorld(float newTickRate = 60);

	/**
	 * @brief Destructor.
	 *
	 * Currently defaulted. All cleanup should be handled in shutdown().
	 */
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
	void fixedUpdate() override;

	/**
	 * @brief Shuts down the physics world.
	 *
	 * All Box2D bodies are destroyed and the world is cleared. After this call,
	 * the world must be restarted with start() to simulate again.
	 */
	void shutdown() override;

	/**
	 * @brief Creates a physics body for the given GameObject.
	 *
	 * The method inspects the RigidBody's Collider and RigidBody components
	 * to configure and create the appropriate Box2D body.
	 *
	 * @param rigidBody Pointer to the RigidBody containing physics components.
	 */
	void createBody(const RigidBody* rigidBody) override;

	/**
	 * @brief Destroys a previously created physics body.
	 *
	 * Removes the body from the physics world and internal mappings.
	 *
	 * @param rigidBody Pointer to the RigidBody whose body should be destroyed.
	 */
	void destroyBody(const RigidBody* rigidBody) override;

	/**
	 * @brief Applies a force to the center of mass of a physics body.
	 *
	 * @param rigitBody Pointer to the RigidBody whose body will receive the force.
	 * @param force Force vector in world units.
	 */
	void applyForce(const RigidBody* rigitBody, Vector2 force) override;

	/**
	 * @brief Synchronizes all registered RigidBody transforms with the physics world.
	 *
	 * After stepping the simulation, this function updates each RigidBody's
	 * Transform component to match the corresponding Box2D body's position and rotation.
	 */
	void syncTransforms() override;

private:
	/**
	 * @brief Box2D world identifier.
	 *
	 * Created in start() and used for all subsequent Box2D operations.
	 */
	b2WorldId worldId;

	/**
	 * @brief Mapping from RigidBody pointers to their corresponding Box2D bodies.
	 *
	 * This allows direct RigidBody-based operations without needing a separate body ID map.
	 */
	std::unordered_map<const RigidBody*, b2BodyId> bodies;


	/**
	 * @brief The amount of ticks to calculate
	 */
	float tickRate;
};