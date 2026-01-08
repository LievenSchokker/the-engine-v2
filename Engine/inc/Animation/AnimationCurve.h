#pragma once

#include "Animation/AnimationEnums.h"
#include "Networking/Serialization/ISerializable.h"
#include "Networking/Serialization/Serialization.h"

/**
 * @class AnimationCurve
 * @brief Represents an easing curve for animation interpolation.
 *
 * AnimationCurve evaluates easing functions at normalized time values [0, 1]
 * to create smooth animation transitions.
 */
class AnimationCurve : public ISerializable
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

    void serialize(WriteArchive& archive) const override;
    void deserialize(ReadArchive& archive) override;

private:
	EasingType type;
};
