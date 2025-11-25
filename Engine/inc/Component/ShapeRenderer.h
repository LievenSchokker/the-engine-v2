#pragma once

#include "../GameObject/Vector2.h"
#include "../Rendering/Color.h"
#include "../Rendering/IRenderer.h"
#include "Component.h"

/**
 * @brief Simple component that renders primitive shapes for a GameObject.
 */
class ShapeRenderer: public Component
{
   public:
	enum class ShapeType { None, Circle, Rectangle };

	ShapeRenderer() = default;

	ShapeRenderer& setColor(const Color& newColor);
	ShapeRenderer& setCircle(float newRadius);
	ShapeRenderer& setRectangle(Vector2 newSize);

	Color getColor() const;
	float getRadius() const;
	Vector2 getSize() const;
	ShapeType getShapeType() const;

	/**
	 * @brief Draws the configured shape using the owning transform.
	 */
	void render(IRenderer& renderer) const;

   private:
	Color color = Color::white();
	float radius = 25.0;
	Vector2 size = {50.0, 50.0};
	ShapeType type = ShapeType::None;
};
