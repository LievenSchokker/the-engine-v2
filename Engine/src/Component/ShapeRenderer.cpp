#include "Component/ShapeRenderer.h"

#include "Component/Transform.h"
#include "../../inc/Math/Vector2Utils.h"

#include <algorithm>

namespace
{
constexpr double kEpsilon = 1e-6;
}

ShapeRenderer& ShapeRenderer::setColor(const Color& newColor)
{
	color = newColor;
	return *this;
}

ShapeRenderer& ShapeRenderer::setCircle(double newRadius)
{
	type = ShapeRenderType::Circle;
	radius = std::max(newRadius, kEpsilon);
	return *this;
}

ShapeRenderer& ShapeRenderer::setRectangle(Vector2 newSize)
{
	type = ShapeRenderType::Rectangle;
	size.x = std::max(newSize.x, kEpsilon);
	size.y = std::max(newSize.y, kEpsilon);
	return *this;
}

Color ShapeRenderer::getColor() const
{
	return color;
}

double ShapeRenderer::getRadius() const
{
	return radius;
}

Vector2 ShapeRenderer::getSize() const
{
	return size;
}

ShapeRenderType ShapeRenderer::getShapeType() const
{
	return type;
}

std::optional<ShapeRenderCommand> ShapeRenderer::buildRenderCommand() const
{
	const Transform* transform = getTransform();
	if ( transform == nullptr || type == ShapeRenderType::None ) {
		return std::nullopt;
	}

	const Vector2 position = transform->getPosition();
	const double rotation = transform->getRotationAngle();
	const Vector2 scale = Vector2Utils::sanitizeScale(transform->getScale());

	ShapeRenderCommand command;
	command.type = type;
	command.position = position;
	command.size = size;
	command.radius = radius;
	command.rotationDegrees = rotation;
	command.scale = scale;
	command.color = color;

	return command;
}
