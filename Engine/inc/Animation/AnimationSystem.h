#pragma once

class Animator;
class Scene;

/**
 * @class AnimationSystem
 * @brief Manages and updates all Animator components in the scene.
 *
 * AnimationSystem finds and updates Animator components by iterating
 * through the Scene's GameObjects, similar to how Behaviour components
 * are updated. This pull-model approach is consistent with other engine
 * systems and avoids manual registration/deregistration.
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
	 * @brief Updates all Animator components in the given scene.
	 *
	 * Iterates through all GameObjects in the scene and updates any
	 * Animator components that are playing and on active GameObjects.
	 * This pull-model approach is consistent with Behaviour updates.
	 *
	 * @param scene The scene containing GameObjects with Animator components
	 * @param deltaTime Time elapsed since last update in seconds
	 */
	void update(Scene* scene, float deltaTime);
};
