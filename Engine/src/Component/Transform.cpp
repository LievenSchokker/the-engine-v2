#include "Component/Transform.h"

Transform::Transform(const Vector2 position, const double rotationAngle, const Vector2 scale) :
    position(position), rotationAngle(rotationAngle), scale(scale){}

Transform::~Transform() = default;

Vector2 Transform::getPosition() const {
    return position;
}

float Transform::getRotationAngle() const {
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

    updateDirectionVectors();
}

void Transform::setScale(const Vector2 newScale) {
    scale = newScale;
}


void Transform::serialize(WriteArchive& archive) const
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

void Transform::deserialize(ReadArchive& archive)
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
void Transform::moveTowards(Vector2 targetPosition, float maxDistance)
{
    Vector2 direction = targetPosition - position;
    float distance = direction.magnitude();

    if (distance <= maxDistance || distance == 0.0f)
    {
        position = targetPosition;
    }
    else
    {
        position += direction.normalised() * maxDistance;
    }
}


void Transform::rotateTowards(const Vector2& targetDirection, float maxRotationSpeed, float deltaTime)
{
    if (targetDirection.magnitude() == 0.0f)
        return;

    // Target angle in degrees
    float targetAngle = std::atan2(targetDirection.y, targetDirection.x) * 180.0f / 3.14159265f;

    // Compute delta and wrap to [-180, 180] for shortest rotation path
    float delta = targetAngle - rotationAngle;
    while (delta > 180.0f) delta -= 360.0f;
    while (delta < -180.0f) delta += 360.0f;

    // Clamp rotation step by max rotation speed
    float step = std::clamp(delta, -maxRotationSpeed * deltaTime, maxRotationSpeed * deltaTime);

    setRotationAngle(rotationAngle + step);
}

void Transform::updateDirectionVectors()
{
    float c = cos(rotationAngle);
    float s = sin(rotationAngle);

    forwardVector = {-s, c};
    rightVector   = { c, s};
}


const Vector2& Transform::forward() const
{
    return forwardVector;
}

const Vector2& Transform::right() const
{
    return rightVector;
}

