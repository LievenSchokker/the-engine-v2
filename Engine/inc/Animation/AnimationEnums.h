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
	Transform,
	Sprite
};

/**
 * @brief Property type for animation tracks.
 *
 * PropertyType specifies which property of a TargetType component to animate.
 * For example:
 * - TargetType::Transform + PropertyType::Position = animate transform position
 * - TargetType::Transform + PropertyType::Rotation = animate transform rotation
 * - TargetType::Transform + PropertyType::Scale = animate transform scale
 * - TargetType::Sprite + PropertyType::Frame = animate sprite frame index
 * - TargetType::Sprite + PropertyType::Offset = animate sprite render offset
 * - TargetType::Sprite + PropertyType::FlipX = animate sprite horizontal flip
 * - TargetType::Sprite + PropertyType::FlipY = animate sprite vertical flip
 */
enum class PropertyType
{
	Position,  // Transform: position (Vector2)
	Rotation,  // Transform: rotation angle (float)
	Scale,	   // Transform: scale (Vector2)
	Frame,	   // Sprite: frame index (int)
	Offset,	   // Sprite: render offset (Vector2)
	FlipX,	   // Sprite: horizontal flip (int: 0=false, 1=true)
	FlipY	   // Sprite: vertical flip (int: 0=false, 1=true)
};
