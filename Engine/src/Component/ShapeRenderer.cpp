#include "Component/ShapeRenderer.h"

#include "Component/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector2Utils.h"

#include <algorithm>

namespace
{
constexpr float kEpsilon = 1e-6;
}

ShapeRenderer& ShapeRenderer::setColor(const Color& newColor)
{
	color = newColor;
	return *this;
}

ShapeRenderer& ShapeRenderer::setCircle(float newRadius)
{
	type = RenderCommandType::Circle;
	radius = std::max(newRadius, kEpsilon);
	return *this;
}

ShapeRenderer& ShapeRenderer::setRectangle(Vector2 newSize)
{
    type = RenderCommandType::Rectangle;

    constexpr float kEpsilon = 0.0001f;

    size.x = std::max(newSize.x, kEpsilon);
    size.y = std::max(newSize.y, kEpsilon);

    return *this;
}

Color ShapeRenderer::getColor() const
{
	return color;
}

float ShapeRenderer::getRadius() const
{
	return radius;
}

Vector2 ShapeRenderer::getSize() const
{
	return size;
}

RenderCommandType ShapeRenderer::getShapeType() const
{
	return type;
}

void ShapeRenderer::fillRenderQueue(IRenderQueueWriter& queue) const
{
	const Transform* transform = getTransform();
	if ( transform == nullptr || type == RenderCommandType::None ) {
		return;
	}

	const Vector2 position = transform->getPosition();
	const double rotation = transform->getRotationAngle();
	const Vector2 scale = Vector2Utils::sanitizeScale(transform->getScale());

	RenderCommand command;
	command.type = type;
	command.position = position;
	command.size = size;
	command.radius = radius;
	command.rotationDegrees = rotation;
	command.scale = scale;
	command.color = color;
	command.layer = layer;
	command.orderInLayer = orderInLayer;
	queue.push(command);
}


void ShapeRenderer::serialize(WriteArchive& archive) const
{
	uint8_t shapeType = static_cast<uint8_t>(type);
	archive.process(shapeType);

	uint8_t r = color.r, g = color.g, b = color.b, a = color.a;
	archive.process(r);
	archive.process(g);
	archive.process(b);
	archive.process(a);

	float rad = radius;
	float sizeX = size.x;
	float sizeY = size.y;
	archive.process(rad);
	archive.process(sizeX);
	archive.process(sizeY);

	uint8_t lay = layer;
	int8_t order = orderInLayer;
	archive.process(lay);
	archive.process(order);
}

ComponentType ShapeRenderer::getComponentType() const
{
	return ComponentType::ShapeRenderer;
}
void ShapeRenderer::deserialize(ReadArchive& archive)
{
	// Shape type
	uint8_t shapeType;
	archive.process(shapeType);
	type = static_cast<RenderCommandType>(shapeType);

	// Color (RGBA)
	uint8_t r, g, b, a;
	archive.process(r);
	archive.process(g);
	archive.process(b);
	archive.process(a);
	color = Color(r, g, b, a);

	// Dimensions
	archive.process(radius);
	archive.process(size.x);
	archive.process(size.y);

	// Layer info
	archive.process(layer);
	archive.process(orderInLayer);
}