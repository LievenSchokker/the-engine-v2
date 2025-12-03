#pragma once

#include "Animation/AnimationClip.h"
#include "Component/Component.h"

class AnimationSystem;
class GameObject;

/**
 * @class Animator
 * @brief Component that plays AnimationClips on a GameObject.
 *
 * Animator manages the playback state of animation clips, including
 * play/pause/stop controls and time scaling.
 */
class Animator: public Component
{
   public:
	/**
	 * @brief Constructs an Animator component.
	 */
	Animator();

	/**
	 * @brief Destructor. Unregisters from AnimationSystem if registered.
	 */
	~Animator() override;

	/**
	 * @brief Called when component is destroyed. Unregisters from
	 * AnimationSystem.
	 */
	void onDestroy() override;

	/**
	 * @brief Plays the specified animation clip.
	 *
	 * If a clip is already playing, it will be stopped and the new clip
	 * started. The animator will register itself with the AnimationSystem if
	 * not already registered.
	 *
	 * @param clip Pointer to the AnimationClip to play (can be nullptr to stop)
	 */
	void play(AnimationClip* clip);

	/**
	 * @brief Pauses the currently playing animation.
	 *
	 * The animation can be resumed by calling resume() to continue from where
	 * it paused.
	 */
	void pause();

	/**
	 * @brief Resumes a paused animation from where it was paused.
	 *
	 * Does nothing if the animation is already playing or if there's no current
	 * clip.
	 */
	void resume();

	/**
	 * @brief Stops the currently playing animation and resets to the beginning.
	 */
	void stop();

	/**
	 * @brief Gets the currently playing clip.
	 * @return Pointer to the current clip, or nullptr if none
	 */
	AnimationClip* getCurrentClip() const;

	/**
	 * @brief Checks if an animation is currently playing.
	 * @return true if playing, false otherwise
	 */
	bool getIsPlaying() const;

	/**
	 * @brief Gets the time scale multiplier.
	 * @return Time scale (1.0 = normal speed)
	 */
	float getTimeScale() const;

	/**
	 * @brief Sets the time scale multiplier.
	 *
	 * @param scale Time scale (1.0 = normal, 2.0 = 2x speed, 0.5 = half speed,
	 * 0.0 = paused)
	 */
	void setTimeScale(float scale);

	/**
	 * @brief Gets the current playback time in the clip.
	 * @return Current time in seconds
	 */
	float getCurrentTime() const;

	/**
	 * @brief Sets the AnimationSystem this Animator should register with.
	 *
	 * This should be called by the AnimationSystem or SceneManager when the
	 * Animator is created.
	 *
	 * @param system Pointer to the AnimationSystem
	 */
	void setAnimationSystem(AnimationSystem* system);

	/**
	 * @brief Clears the reference to AnimationSystem without unregistering.
	 *
	 * Used when AnimationSystem is being destroyed to prevent dangling
	 * pointers. Internal use only.
	 */
	void clearAnimationSystem();

	/**
	 * @brief Internal method called by AnimationSystem to update this animator.
	 *
	 * @param deltaTime Time elapsed since last update in seconds
	 */
	void update(float deltaTime);

   private:
	AnimationClip* currentClip;
	bool isPlaying;
	float timeScale;
	float currentTime;
	AnimationSystem* animationSystem;
	bool isRegistered;

	/**
	 * @brief Registers this animator with the AnimationSystem.
	 */
	void registerWithSystem();

	/**
	 * @brief Unregisters this animator from the AnimationSystem.
	 */
	void unregisterFromSystem();
};
