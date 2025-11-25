#pragma once

#include "IPhysicsWorld.h"
#include "Box2D/Box2DPhysicsWorld.h"
#include "GameObject/Vector2.h"

#include <memory>
#include <unordered_map>

/**
 * @class PhysicsSystem
 * @brief High-level manager for the physics simulation.
 *
 * The PhysicsSystem owns a Box2D-based physics world and coordinates all
 * physics-related operations.
 *
 * Responsibilities:
 * - Creating and initializing the underlying physics world.
 * - Registering and unregistering GameObjects that should participate in physics.
 * - Stepping the physics simulation.
 * - Synchronizing simulated transforms back to GameObjects.
 * - Applying forces or impulses to registered bodies.
 */
class PhysicsSystem
{
public:
	/**
	 * @brief Constructs the physics system and creates the physics world.
	 *
	 * No bodies are registered at construction time; they must be added explicitly
	 * using registerBody().
	 */
	PhysicsSystem();
	~PhysicsSystem() = default;

	/**
	 * @brief Initializes the physics world.
	 *
	 * This must be called before any update() or physics interaction occurs.
	 * Typically called once at application startup or scene load.
	 */
	void start() const;

	/**
	 * @brief Advances the physics simulation by one time step.
	 *
	 * @param deltaTime Time in seconds since the last update.
	 *        The underlying physics world may internally apply fixed-step logic.
	 */
	void update(float deltaTime) const;


	/**
	 * @brief Registers a GameObject with the physics system.
	 *
	 * The system will read its Collider and RigidBody components to create a
	 * corresponding body inside the physics world.
	 *
	 * If registration succeeds, the body will be simulated automatically and
	 * included in syncData() calls.
	 *
	 * @param gameObject The GameObject to register. Must not be null.
	 */
	void registerBody(const GameObject* gameObject);


	/**
	 * @brief Removes a previously registered GameObject from physics simulation.
	 *
	 * This destroys the associated body inside the physics world and removes it
	 * from the internal tracking map.
	 *
	 * @param gameObject The GameObject to unregister. Must not be null.
	 */
	void unregisterBody(const GameObject* gameObject);


	/**
	 * @brief Synchronizes simulated body transforms back to their GameObjects.
	 *
	 * After stepping the physics simulation, this method updates each registered
	 * GameObject's Transform component with the latest position and rotation from
	 * the physics world.
	 */
	void syncData();


	/**
	 * @brief Applies a continuous force to a registered GameObject's physics body.
	 *
	 * The force is applied at the body's center of mass.
	 * If the GameObject is not registered or lacks a valid physics body,
	 * this function has no effect.
	 *
	 * @param gameObject The target GameObject.
	 * @param force The force vector to apply.
	 */
	void applyForce(const GameObject* gameObject, Vector2 force);

private:
	/**
	 * @brief The underlying Box2D physics world instance.
	 */
	std::unique_ptr<Box2DPhysicsWorld> world;

	/**
	 * @brief Mapping of registered GameObjects to their corresponding Box2D body IDs.
	 *
	 * Only bodies that have been successfully created and registered will appear here.
	 */
	std::unordered_map<const GameObject*, b2BodyId> physicObjects;
};