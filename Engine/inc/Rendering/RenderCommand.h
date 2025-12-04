#pragma once


#include "Color.h"
#include "RenderCommandType.h"
#include "GameObject/Vector2.h"
#include "Math/Vector2.h"


/**
 * @brief Describes a single primitive shape ready for the renderer to consume.
 */
struct RenderCommand {
	RenderCommandType type = RenderCommandType::None;


	Vector2 position{0.0, 0.0};
	Vector2 size{0.0, 0.0};
	double radius = 0.0;
	double rotationDegrees = 0.0;
	Vector2 scale{1.0, 1.0};
	Color color = Color::white();

	uint8_t layer = 0;
	uint8_t orderInLayer = 0;

	int getSortKey() const
	{
		return (layer << 8) | orderInLayer;
	}
};
