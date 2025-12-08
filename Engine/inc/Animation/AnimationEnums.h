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
