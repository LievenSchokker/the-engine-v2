#include "../../../include/core/game_object/Transform.h"

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

