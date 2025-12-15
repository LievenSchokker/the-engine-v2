#pragma once

#include "Animation/AnimationCurve.h"
#include "Animation/AnimationEnums.h"
#include "Math/Vector2.h"

#include <variant>

class Animator;
class Transform;

/**
 * @class AnimationTrack
 * @brief Represents a single property animation track.
 *
 * An AnimationTrack animates a specific property (Position, Rotation, Scale,
 * Frame) of a Transform or Sprite component over a duration using an easing
 * curve.
 */
class AnimationTrack
{
   public:
	/**
	 * @brief Constructs an AnimationTrack.
	 *
	 * @param target The target type (Transform or Sprite)
	 * @param property The property to animate
	 * @param duration Duration in seconds
	 * @param relative If true, animates relative to current value
	 * @param fromValue Starting value (Vector2 for Position/Scale/Offset, float
	 * for Rotation, int for Frame/FlipX/FlipY where 0=false, 1=true)
	 * @param toValue Ending value (Vector2 for Position/Scale/Offset, float for
	 * Rotation, int for Frame/FlipX/FlipY where 0=false, 1=true)
	 * @param curve The easing curve to use
	 */
	AnimationTrack(TargetType target, PropertyType property, float duration,
				   bool relative, std::variant<Vector2, float, int> fromValue,
				   std::variant<Vector2, float, int> toValue,
				   AnimationCurve curve);

	/**
	 * @brief Samples the animation value at normalized time.
	 *
	 * @param t Normalized time in range [0, 1]
	 * @return Interpolated value (Vector2, float, or int)
	 */
	std::variant<Vector2, float, int> sample(float t) const;

	/**
	 * @brief Applies the sampled value to the Animator's GameObject's Transform
	 * at the given normalized time.
	 *
	 * @param animator The Animator component whose GameObject's Transform will
	 * be modified
	 * @param normalizedTime Normalized time [0, 1] for this track (currentTime
	 * / duration)
	 */
	void apply(Animator* animator, float normalizedTime) const;

	/**
	 * @brief Gets the target type.
	 * @return The TargetType
	 */
	TargetType getTarget() const;

	/**
	 * @brief Gets the property type.
	 * @return The PropertyType
	 */
	PropertyType getProperty() const;

	/**
	 * @brief Gets the duration.
	 * @return Duration in seconds
	 */
	float getDuration() const;

	/**
	 * @brief Checks if this track is relative.
	 * @return true if relative, false if absolute
	 */
	bool isRelative() const;

	/**
	 * @brief Gets the starting value.
	 * @return The fromValue variant
	 */
	const std::variant<Vector2, float, int>& getFromValue() const;

	/**
	 * @brief Gets the ending value.
	 * @return The toValue variant
	 */
	const std::variant<Vector2, float, int>& getToValue() const;

	/**
	 * @brief Gets the animation curve.
	 * @return The AnimationCurve
	 */
	const AnimationCurve& getCurve() const;

   private:
	TargetType target;
	PropertyType property;
	float duration;
	bool relative;	// If true, the fromValue is an offset. If false, the
					// fromValue is an absolute value.
	std::variant<Vector2, float, int> fromValue;
	std::variant<Vector2, float, int> toValue;
	AnimationCurve curve;
};
