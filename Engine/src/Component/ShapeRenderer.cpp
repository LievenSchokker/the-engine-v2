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
	if ( transform == nullptr || type == RenderCommandType::None )
	{
		return;
	}

	// Use world transforms to respect parent-child hierarchy
	const Vector2 position = transform->getWorldPosition();
	const double rotation = transform->getWorldRotation();
	const Vector2 scale =
		Vector2Utils::sanitizeScale(transform->getWorldScale());

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
