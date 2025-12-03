#pragma once

#include "Animation/AnimationClip.h"
#include "Component/Component.h"

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
	 * @brief Destructor.
	 */
	~Animator() override;

	/**
	 * @brief Called when component is destroyed.
	 */
	void onDestroy() override;

	/**
	 * @brief Plays the specified animation clip.
	 *
	 * If a clip is already playing, it will be stopped and the new clip
	 * started. The AnimationSystem will automatically find and update this
	 * animator each frame (pull-model, consistent with Behaviour components).
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
	 * @brief Internal method called by AnimationSystem to update this animator.
	 *
	 * AnimationSystem finds this animator by iterating through Scene's
	 * GameObjects (pull-model, consistent with Behaviour components).
	 *
	 * @param deltaTime Time elapsed since last update in seconds
	 */
	void update(float deltaTime);

   private:
	AnimationClip* currentClip;
	bool isPlaying;
	float timeScale;
	float currentTime;
};
