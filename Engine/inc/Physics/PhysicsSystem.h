#pragma once
#include "IPhysicsWorld.h"
#include "Core/IEngineSystems.h"


class PhysicsSystem : public IEngineSystems
{
public:

	PhysicsSystem();
	/**
	 * @brief Initializes the Box2D world.
	 *
	 * This creates the internal b2WorldId instance and applies default
	 * world settings such as gravity. Must be called before update() or
	 * body creation.
	 */
	SystemStatus start(GameWorld& gameWorld) override;

	/**
	 * @brief Steps the Box2D simulation forward by one fixed timestep.
	 *
	 * The timestep and substep count are typically configured internally.
	 * This method is called by PhysicsSystem::update().
	 */
	void fixedUpdate(double deltaTime, const GameWorld& gameWorld) override;

	/**
	 * @brief Shuts down the physics world.
	 *
	 * All Box2D bodies are destroyed and the world is cleared. After this call,
	 * the world must be restarted with start() to simulate again.
	 */
	void shutdown(GameWorld& gameWorld) override;
	const std::string getName() const override;
	IPhysicsWorld* getPhysicsWorld() const;

private:
	std::unique_ptr<IPhysicsWorld> physicsWorld;
};