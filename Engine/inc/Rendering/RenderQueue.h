#pragma once

#include "../Math/Vector2.h"
#include "Color.h"

#include <vector>

class IRenderer;

/**
 * @brief Enumeration of primitive shapes the engine can draw via ShapeRenderer.
 */
enum class ShapeRenderType { None, Circle, Rectangle };

/**
 * @brief Describes a single primitive shape ready for the renderer to consume.
 */
struct ShapeRenderCommand {
	ShapeRenderType type = ShapeRenderType::None;
	Vector2 position = Vector2::zero();
	Vector2 size = Vector2::zero();
	double radius = 0.0;
	double rotationDegrees = 0.0;
	Vector2 scale{1.0, 1.0};
	Color color = Color::white();
};

/**
 * @brief Lightweight container that stores all primitives scheduled for a
 * frame.
 */
struct RenderQueue {
	void clear()
	{
		shapes.clear();
	}

	Color clearColor = Color::black();
	std::vector<ShapeRenderCommand> shapes;
};

/**
 * @brief Executes the provided render queue on the supplied renderer.
 */
void executeRenderQueue(IRenderer& renderer, const RenderQueue& queue);
