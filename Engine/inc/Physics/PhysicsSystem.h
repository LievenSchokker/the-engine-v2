#pragma once

#include "IPhysicsWorld.h"
#include "Box2D/Box2DPhysicsWorld.h"

#include <memory>
#include <unordered_map>

/**
 * @class PhysicsSystem
 * @brief Manages the physics world and all dynamic bodies within it.
 *
 * This system owns and updates a Box2D-based physics world. It allows external
 * objects to register/unregister dynamic bodies, steps the simulation forward
 * in time, and synchronizes simulated data back into the associated components.
 */
class PhysicsSystem
{
public:
	/**
	 * @brief Constructs the physics system and initializes the physics world.
	 */
	PhysicsSystem();
	~PhysicsSystem() = default;

	void start() const;

	/**
	 * @brief Steps the physics simulation forward by a given delta time.
	 *
	 * @param deltaTime The time step (in seconds) used for advancing the simulation.
	 */
	void update(float deltaTime) const;


	/**
	 * @brief Registers a dynamic body with the physics system.
	 *
	 * The body will be added to the internal list of dynamic bodies and
	 * included in physics simulation steps.
	 *
	 * @param object Pointer to the dynamic body to register.
	 */
	void registerBody(const GameObject* object);


	/**
	 * @brief Unregisters a previously registered dynamic body.
	 *
	 * The body will be removed from the internal list and will no longer
	 * be updated by the physics simulation.
	 *
	 * @param obj Pointer to the dynamic body to unregister.
	 */
	void unregisterBody(GameObject* gameObject);


	/**
	 * @brief Synchronizes data from the physics world back into the game objects.
	 *
	 * This should be called after stepping the simulation to update the
	 * corresponding transform or state of each DynamicBody.
	 */
	void syncData();

private:
	/**
	 * @brief The underlying Box2D physics world instance.
	 */
	std::unique_ptr<Box2DPhysicsWorld> world;

	/**
	 * @brief List of all dynamic bodies currently registered with the physics system.
	 */
	std::unordered_map<const GameObject*, b2BodyId> physicObjects;
};