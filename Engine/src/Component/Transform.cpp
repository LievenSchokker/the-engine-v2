#include "Component/Transform.h"

Transform::Transform(const Vector2 position, const double rotationAngle, const Vector2 scale) :
    position(position), rotationAngle(rotationAngle), scale(scale){}

Transform::~Transform() = default;

Vector2 Transform::getPosition() const {
    return position;
}

double Transform::getRotationAngle() const {
    return rotationAngle;
}

Vector2 Transform::getScale() const {
    return scale;
}

void Transform::setPosition(const Vector2 newPosition) {
    position = newPosition;
}


void Transform::setRotationAngle(const double newRotationAngle) {
    rotationAngle = newRotationAngle;
}

void Transform::setScale(const Vector2 newScale) {
    scale = newScale;
}


void Transform::serialize(CerealWriteArchive& archive) const
{
	// Position
	float posX = static_cast<float>(position.x);
	float posY = static_cast<float>(position.y);
	archive.process(posX);
	archive.process(posY);

	// Rotation
	double rotation = rotationAngle;
	archive.process(rotation);

	// Scale
	float scaleX = static_cast<float>(scale.x);
	float scaleY = static_cast<float>(scale.y);
	archive.process(scaleX);
	archive.process(scaleY);
}

void Transform::deserialize(CerealReadArchive& archive)
{
	// Position
	float posX, posY;
	archive.process(posX);
	archive.process(posY);
	position = {posX, posY};

	// Rotation
	archive.process(rotationAngle);

	// Scale
	float scaleX, scaleY;
	archive.process(scaleX);
	archive.process(scaleY);
	scale = {scaleX, scaleY};
}