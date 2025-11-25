#include "Component/ShapeRenderer.h"

#include "Component/Transform.h"
#include "GameObject/Vector2Utils.h"

#include <algorithm>
#include <cmath>

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
	type = ShapeType::Circle;
	radius = std::max(newRadius, kEpsilon);
	return *this;
}

ShapeRenderer& ShapeRenderer::setRectangle(Vector2 newSize)
{
	type = ShapeType::Rectangle;
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

ShapeRenderer::ShapeType ShapeRenderer::getShapeType() const
{
	return type;
}

void ShapeRenderer::render(IRenderer& renderer) const
{
	const Transform* transform = getTransform();
	if ( transform == nullptr || type == ShapeType::None ) {
		return;
	}

	const Vector2 position = transform->getPosition();
	const double rotation = transform->getRotationAngle();
	const Vector2 scale = Vector2Utils::sanitizeScale(transform->getScale());

	if ( type == ShapeType::Circle ) {
		renderer.drawCircle(position, radius, color, scale);
		return;
	}

	renderer.drawRectangle(position, size, rotation, color, scale);
}
