#pragma once

#include "Math/Vector2.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "BaseComponentTypes/RenderComponent.h"

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

	ShapeRenderer& setLayer(uint8_t l)
	{
		layer = l;
		return *this;
	}

	Color getColor() const;
	float getRadius() const;
	Vector2 getSize() const;
	RenderCommandType getShapeType() const;

	/**
	 * @brief Builds a render command that the renderer can consume later.
	 */
	void fillRenderQueue(IRenderQueueWriter& queue) const override;

	ComponentType getComponentType() const override;
	void serialize(CerealWriteArchive& archive) const override;
	void deserialize(CerealReadArchive& archive) override;
private:
	Color color = Color::white();
	float radius = 25.0;
	Vector2 size = {50.0, 50.0};
	RenderCommandType type = RenderCommandType::None;
	uint8_t layer = 0;
	int8_t orderInLayer = 0;
};