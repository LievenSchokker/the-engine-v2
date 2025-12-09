#pragma once

#include "Animation/AnimationClip.h"
#include "Behaviour/Behaviour.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

class GameObject;
struct SpritesheetAnimationClip;

/**
 * @class Animator
 * @brief Behaviour component that plays AnimationClips on a GameObject.
 *
 * Animator manages the playback state of animation clips, including
 * play/pause/stop controls and time scaling. It inherits from Behaviour
 * so Scene automatically updates it each frame along with other behaviours.
 */
class Animator: public Behaviour
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
	 * started. Scene will automatically update this animator each frame
	 * along with other Behaviour components.
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
	 * @brief Updates the animator each frame.
	 *
	 * Called automatically by Scene when this Behaviour is enabled and active.
	 * Advances animation time and applies tracks based on current playback
	 * state.
	 *
	 * @param deltaTime Time elapsed since last update in seconds
	 */
	void update(float deltaTime, GameWorld* world) override;

	/**
	 * @brief Adds a spritesheet animation clip that can be played by name.
	 *
	 * Converts the SpritesheetAnimationClip to an AnimationClip internally
	 * and stores it for later playback. This provides a convenient API for
	 * spritesheet animations without requiring manual AnimationTrack creation.
	 *
	 * @param name Name to identify this animation (used with playSpritesheet())
	 * @param clip SpritesheetAnimationClip definition
	 */
	void addSpritesheetClip(const std::string& name,
							const SpritesheetAnimationClip& clip);

	/**
	 * @brief Plays a spritesheet animation by name.
	 *
	 * Stops any currently playing animation and starts the named spritesheet
	 * animation. Does nothing if the animation name doesn't exist.
	 *
	 * @param clipName Name of the spritesheet animation to play
	 */
	void playSpritesheet(const std::string& clipName);

	/**
	 * @brief Gets the name of the currently playing spritesheet animation.
	 * @return Animation name, or empty string if none playing or current clip
	 * is not a spritesheet clip
	 */
	std::string getCurrentSpritesheetClipName() const;

	/**
	 * @brief Checks if a spritesheet clip exists.
	 * @param name Name of the clip to check
	 * @return true if clip exists, false otherwise
	 */
	bool hasSpritesheetClip(const std::string& name) const;

	/**
	 * @brief Adds sprite frame tracks from a SpritesheetAnimationClip to an
	 * existing AnimationClip.
	 *
	 * This allows combining sprite frame animation with other property
	 * animations (like transform position) in a single clip. The frame tracks
	 * will loop continuously throughout the clip's duration.
	 *
	 * @param targetClip The AnimationClip to add tracks to
	 * @param spritesheetClip The SpritesheetAnimationClip containing frame data
	 */
	void addSpritesheetTracksToClip(
		AnimationClip* targetClip,
		const SpritesheetAnimationClip& spritesheetClip);

   private:
	AnimationClip* currentClip;
	bool isPlaying;
	float timeScale;
	float currentTime;

	/// Storage for spritesheet clips converted from SpritesheetAnimationClip
	std::unordered_map<std::string, std::unique_ptr<AnimationClip>>
		spritesheetClips;

	/// Reverse mapping from AnimationClip pointer to spritesheet clip name for
	/// O(1) lookup
	std::unordered_map<AnimationClip*, std::string> clipToNameMap;

	/// Clips that should loop their Frame tracks automatically
	std::unordered_set<AnimationClip*> clipsWithLoopingFrames;

	/**
	 * @brief Converts a SpritesheetAnimationClip to an AnimationClip.
	 *
	 * Creates sequential AnimationTracks for each frame transition in the
	 * frame sequence. Each track animates from one frame to the next over
	 * the specified frame duration.
	 *
	 * @param clip The SpritesheetAnimationClip to convert
	 * @return Unique pointer to the converted AnimationClip
	 */
	std::unique_ptr<AnimationClip> convertSpritesheetClip(
		const SpritesheetAnimationClip& clip);

	/**
	 * @brief Adds frame tracks from a SpritesheetAnimationClip to an
	 * AnimationClip.
	 *
	 * Helper method that creates and adds frame animation tracks. Can repeat
	 * the frame sequence multiple times.
	 *
	 * @param clip The AnimationClip to add tracks to
	 * @param spritesheetClip The SpritesheetAnimationClip containing frame data
	 * @param repeatCount Number of times to repeat the frame sequence (default:
	 * 1)
	 */
	void addFrameTracksToClip(AnimationClip* clip,
							  const SpritesheetAnimationClip& spritesheetClip,
							  int repeatCount = 1);
};
