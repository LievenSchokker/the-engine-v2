#pragma once

#include <vector>

class Animator;

/**
 * @class AnimationSystem
 * @brief Manages and updates all Animator components in the scene.
 *
 * AnimationSystem is responsible for registering Animators and updating
 * them each frame. It should be owned by SceneManager and updated in
 * the main game loop.
 */
class AnimationSystem
{
   public:
	/**
	 * @brief Constructs an AnimationSystem.
	 */
	AnimationSystem();

	/**
	 * @brief Destructor.
	 */
	~AnimationSystem();

	/**
	 * @brief Registers an Animator to be updated each frame.
	 *
	 * @param animator Pointer to the Animator to register
	 */
	void registerAnimator(Animator* animator);

	/**
	 * @brief Unregisters an Animator from updates.
	 *
	 * @param animator Pointer to the Animator to unregister
	 */
	void unregisterAnimator(Animator* animator);

	/**
	 * @brief Updates all registered Animators.
	 *
	 * This should be called each frame with the delta time.
	 * Automatically cleans up destroyed animators.
	 *
	 * @param deltaTime Time elapsed since last update in seconds
	 */
	void step(float deltaTime);

	/**
	 * @brief Gets the number of registered animators.
	 * @return Number of registered animators
	 */
	size_t getAnimatorCount() const;

   private:
	std::vector<Animator*> animators;

	/**
	 * @brief Removes null pointers from the animators vector.
	 *
	 * Called automatically during Step() to clean up destroyed animators.
	 */
	void cleanupDestroyedAnimators();
};
