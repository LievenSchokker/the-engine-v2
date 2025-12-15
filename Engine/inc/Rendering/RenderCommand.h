#pragma once

#include "Color.h"
#include "Math/Vector2.h"
#include "RenderCommandType.h"
#include "Rendering/Rect.h"

class IImage;

/**
 * @brief Describes a single primitive shape or sprite ready for the renderer to
 * consume.
 */
struct RenderCommand
{
	RenderCommandType type = RenderCommandType::None;

	Vector2 position{0.0, 0.0};
	Vector2 size{0.0, 0.0};
	double radius = 0.0;
	double rotationDegrees = 0.0;
	Vector2 scale{1.0, 1.0};
	Color color = Color::white();

	// Sprite-specific fields (only used when type == Sprite)
	IImage* sprite = nullptr;
	Rect srcRect{0, 0, 0, 0};
	Color tint = Color::white();
	bool flipX = false;
	bool flipY = false;

	uint8_t layer = 0;
	uint8_t orderInLayer = 0;

	int getSortKey() const
	{
		return (layer << 8) | orderInLayer;
	}
};