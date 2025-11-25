#pragma once

#include "../GameObject/Vector2.h"
#include "../Rendering/Color.h"
#include "../Rendering/RenderQueue.h"
#include "Component.h"

#include <optional>

/**
 * @brief Simple component that renders primitive shapes for a GameObject.
 */
class ShapeRenderer: public Component
{
   public:
	ShapeRenderer() = default;

	ShapeRenderer& setColor(const Color& newColor);
	ShapeRenderer& setCircle(float newRadius);
	ShapeRenderer& setRectangle(Vector2 newSize);

	Color getColor() const;
	float getRadius() const;
	Vector2 getSize() const;
	ShapeRenderType getShapeType() const;

	/**
	 * @brief Builds a render command that the renderer can consume later.
	 */
	std::optional<ShapeRenderCommand> buildRenderCommand() const;

   private:
	Color color = Color::white();
	float radius = 25.0;
	Vector2 size = {50.0, 50.0};
	ShapeRenderType type = ShapeRenderType::None;
};
