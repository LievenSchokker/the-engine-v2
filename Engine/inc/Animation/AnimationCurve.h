#pragma once

/**
 * @brief Easing function types for animation curves.
 */
enum class EasingType
{
	Linear,
	EaseInQuad,
	EaseOutQuad,
	EaseInOutQuad,
	EaseInCubic,
	EaseOutCubic,
	EaseInOutCubic
};

/**
 * @brief Target type for animation tracks.
 */
enum class TargetType
{
	Transform  // Sprite will be added in Phase 2
};

/**
 * @brief Property type for animation tracks.
 */
enum class PropertyType
{
	Position,
	Rotation,
	Scale
	// Offset, FlipX, FlipY will be added in Phase 2
};

/**
 * @class AnimationCurve
 * @brief Represents an easing curve for animation interpolation.
 *
 * AnimationCurve evaluates easing functions at normalized time values [0, 1]
 * to create smooth animation transitions.
 */
class AnimationCurve
{
   public:
	/**
	 * @brief Constructs an AnimationCurve with the specified easing type.
	 * @param type The easing function type to use
	 */
	explicit AnimationCurve(EasingType type = EasingType::Linear);

	/**
	 * @brief Evaluates the curve at a normalized time value.
	 *
	 * @param x Normalized time value in range [0, 1]
	 * @return Interpolated value in range [0, 1]
	 */
	float evaluate01(float x) const;

	/**
	 * @brief Gets the easing type of this curve.
	 * @return The EasingType of this curve
	 */
	EasingType getType() const;

	/**
	 * @brief Sets the easing type of this curve.
	 * @param type The new EasingType
	 */
	void setType(EasingType type);

   private:
	EasingType type;
};
