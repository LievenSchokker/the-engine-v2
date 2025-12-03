#pragma once

#include "../GameObject/Vector2.h"
#include "../Rendering/Color.h"
#include "../Rendering/RenderQueue/RenderQueue.h"
#include "BaseComponentTypes/Component.h"
#include "BaseComponentTypes/RenderComponent.h"

#include <optional>

/**
 * @brief Simple component that renders primitive shapes for a GameObject.
 */
class ShapeRenderer: public RenderComponent
{
   public:
	ShapeRenderer() = default;

	ShapeRenderer& setColor(const Color& newColor);
	ShapeRenderer& setCircle(float newRadius);
	ShapeRenderer& setRectangle(Vector2 newSize);

	Color getColor() const;
	float getRadius() const;
	Vector2 getSize() const;
	RenderCommandType getShapeType() const;

	/**
	 * @brief Builds a render command that the renderer can consume later.
	 */
	void fillRenderQueue(IRenderQueueWriter& queue) const override;

   private:
	Color color = Color::white();
	float radius = 25.0;
	Vector2 size = {50.0, 50.0};
	RenderCommandType type = RenderCommandType::None;
};
