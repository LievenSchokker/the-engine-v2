#pragma once

#include "Color.h"
#include "Math/Vector2.h"
#include "Rendering/Rect.h"

#include <vector>

class IRenderer;
class IImage;

/**
 * @brief Enumeration of primitive shapes the engine can draw via ShapeRenderer.
 */
enum class ShapeRenderType
{
	None,
	Circle,
	Rectangle
};

/**
 * @brief Describes a single primitive shape ready for the renderer to consume.
 */
struct ShapeRenderCommand
{
	ShapeRenderType type = ShapeRenderType::None;
	Vector2 position = Vector2::zero();
	Vector2 size = Vector2::zero();
	double radius = 0.0;
	double rotationDegrees = 0.0;
	Vector2 scale{1.0, 1.0};
	Color color = Color::white();
};

/**
 * @brief Describes a single sprite render command ready for the renderer to
 * consume.
 */
struct SpriteRenderCommand
{
	IImage* sprite = nullptr;
	Rect srcRect{0, 0, 0, 0};
	Vector2 position{};
	Vector2 size{};
	double rotationDegrees = 0.0;
	Vector2 scale{1.0, 1.0};
	Color tint = Color::white();
	bool flipX = false;
	bool flipY = false;
};

/**
 * @brief Lightweight container that stores all primitives scheduled for a
 * frame.
 */
struct RenderQueue
{
	void clear()
	{
		shapes.clear();
		sprites.clear();
	}

	Color clearColor = Color::black();
	std::vector<ShapeRenderCommand> shapes;
	std::vector<SpriteRenderCommand> sprites;
};

/**
 * @brief Executes the provided render queue on the supplied renderer.
 */
void executeRenderQueue(IRenderer& renderer, const RenderQueue& queue);
