#include "Component/Transform.h"

#include "GameObject/GameObject.h"
#include "Math/Matrix3.h"
#include "Math/Vector2.h"
#include "Networking/Serialization/Serialization.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Transform::Transform(const Vector2 position, const double rotationAngle,
					 const Vector2 scale)
	: position(position),
	  rotationAngle(rotationAngle),
	  scale(scale),
	  parent(nullptr),
	  hasEverHadParent(false),
	  cachedLocalMatrix(Matrix3()),
	  cachedWorldMatrix(Matrix3()),
	  isLocalMatrixDirty(true),
	  isWorldMatrixDirty(true),
	  forwardVector(Vector2(0, 1)),
	  rightVector(Vector2(1, 0))
{
	updateDirectionVectors();
}

Transform::~Transform() = default;

Vector2 Transform::getPosition() const
{
	return position;
}

float Transform::getRotationAngle() const
{
	return rotationAngle;
}

Vector2 Transform::getScale() const
{
	return scale;
}

void Transform::setPosition(const Vector2 newPosition)
{
	if ( position.x == newPosition.x && position.y == newPosition.y ) return;
	position = newPosition;
	markDirty();
}

void Transform::setRotationAngle(const double newRotationAngle)
{
	if ( rotationAngle == newRotationAngle ) return;
	rotationAngle = newRotationAngle;
	updateDirectionVectors();
	markDirty();
}

void Transform::setScale(const Vector2 newScale)
{
	if ( scale.x == newScale.x && scale.y == newScale.y ) return;
	scale = newScale;
	markDirty();
}

Vector2 Transform::getWorldPosition() const
{
	return getWorldMatrix().extractTranslation();
}

double Transform::getWorldRotation() const
{
	if ( parent == nullptr )
	{
		return rotationAngle;
	}
	// World rotation is the sum of local rotation and parent's world rotation
	return rotationAngle + parent->getWorldRotation();
}

Vector2 Transform::getWorldScale() const
{
	if ( parent == nullptr )
	{
		return scale;
	}
	// World scale is the product of local scale and parent's world scale
	const Vector2 parentWorldScale = parent->getWorldScale();
	return Vector2(scale.x * parentWorldScale.x, scale.y * parentWorldScale.y);
}

Matrix3 Transform::getLocalMatrix() const
{
	if ( isLocalMatrixDirty )
	{
		updateLocalMatrix();
	}
	return cachedLocalMatrix;
}

Matrix3 Transform::getWorldMatrix() const
{
	if ( isWorldMatrixDirty )
	{
		updateWorldMatrix();
	}
	return cachedWorldMatrix;
}

bool Transform::setParent(Transform* newParent)
{
	if ( parent == newParent ) return true;

	// Prevent circular parenting
	if ( newParent != nullptr )
	{
		Transform* check = newParent->getParent();
		while ( check != nullptr )
		{
			if ( check == this )
			{
				// Circular reference detected, don't set parent
				return false;
			}
			check = check->getParent();
		}
	}

	// Store current world transform values before changing parent
	Vector2 worldPos = getWorldPosition();
	double worldRot = getWorldRotation();
	Vector2 worldScale = getWorldScale();

	// When detaching from parent, preserve world transform values
	if ( parent != nullptr && newParent == nullptr )
	{
		// Convert world values to local (which equals world when no parent)
		position = worldPos;
		rotationAngle = worldRot;
		scale = worldScale;
	}

	// When attaching to a new parent, convert world transform to local space
	// relative to the new parent
	if ( newParent != nullptr )
	{
		bool shouldConvert = false;

		if ( parent != nullptr )
		{
			// Switching parents - always convert
			shouldConvert = true;
		}
		else
		{
			// No previous parent - check if we've ever had a parent before
			// If we have, this is re-attachment after detach, so convert
			// If we haven't, this is first attachment, so preserve local values
			shouldConvert = hasEverHadParent;
		}

		if ( shouldConvert )
		{
			// Get parent's world transform
			Vector2 parentWorldPos = newParent->getWorldPosition();
			double parentWorldRot = newParent->getWorldRotation();
			Vector2 parentWorldScale = newParent->getWorldScale();

			// Convert world position to local position relative to parent
			// We need to inverse transform the world position by the parent's
			// transform
			Matrix3 parentWorldMatrix = newParent->getWorldMatrix();
			Matrix3 parentInverseMatrix = parentWorldMatrix.inverse();
			Vector2 localPos = parentInverseMatrix * worldPos;
			position = localPos;

			// Convert world rotation to local rotation
			// Local rotation = world rotation - parent world rotation
			rotationAngle = worldRot - parentWorldRot;

			// Convert world scale to local scale
			// Local scale = world scale / parent world scale
			scale = Vector2(worldScale.x / parentWorldScale.x,
							worldScale.y / parentWorldScale.y);
		}
		// If not converting, keep local values as-is
	}

	// Update hasEverHadParent flag
	if ( newParent != nullptr && !hasEverHadParent )
	{
		hasEverHadParent = true;
	}

	parent = newParent;
	markDirty();
	return true;
}

Transform* Transform::getParent() const
{
	return parent;
}

void Transform::markDirty()
{
	// Delegate to GameObject to handle dirty propagation through parent-child
	// hierarchy
	if ( gameObject != nullptr )
	{
		gameObject->markTransformDirty();
	}
	else
	{
		// Fallback: if no GameObject, just mark local dirty
		markDirtyLocal();
	}
}

void Transform::markDirtyLocal()
{
	isLocalMatrixDirty = true;
	isWorldMatrixDirty = true;
}

void Transform::updateLocalMatrix() const
{
	// Build local matrix as T * R * S (translate, then rotate, then scale)
	// This order ensures that when parent rotates, children orbit around parent
	// instead of just rotating in place
	Matrix3 translationMatrix = Matrix3::translation(position);
	Matrix3 rotationMatrix = Matrix3::rotation(rotationAngle);
	Matrix3 scaleMatrix = Matrix3::scale(scale);

	// T * R * S: First translate, then rotate around origin, then scale
	cachedLocalMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	isLocalMatrixDirty = false;
}

void Transform::updateWorldMatrix() const
{
	updateLocalMatrix();

	if ( parent == nullptr )
	{
		cachedWorldMatrix = cachedLocalMatrix;
	}
	else
	{
		// World matrix = parent's world matrix * local matrix
		cachedWorldMatrix = parent->getWorldMatrix() * cachedLocalMatrix;
	}

	isWorldMatrixDirty = false;
}

void Transform::moveTowards(Vector2 targetPosition, float maxDistance)
{
	Vector2 currentPos = getPosition();
	Vector2 direction = targetPosition - currentPos;
	float distance = direction.magnitude();

	if ( distance <= maxDistance )
	{
		// Snap to target if within max distance
		setPosition(targetPosition);
	}
	else
	{
		// Move towards target by maxDistance
		Vector2 moveDirection = direction.normalised();
		Vector2 newPosition = currentPos + moveDirection * maxDistance;
		setPosition(newPosition);
	}
}

void Transform::rotateTowards(const Vector2& targetDirection,
							  float maxRotationSpeed, float deltaTime)
{
	if (targetDirection.magnitude() < 0.001f)
		return;

	float targetAngle = std::atan2(targetDirection.y, targetDirection.x)
						* 180.0f / static_cast<float>(M_PI);

	float angleDiff = targetAngle - rotationAngle;

	while (angleDiff > 180.0f) angleDiff -= 360.0f;
	while (angleDiff < -180.0f) angleDiff += 360.0f;

	float maxRotationThisFrame = maxRotationSpeed * deltaTime;
	float rotationAmount;

	if (std::abs(angleDiff) <= maxRotationThisFrame)
	{
		rotationAmount = angleDiff;
	}
	else
	{
		rotationAmount = (angleDiff > 0) ? maxRotationThisFrame : -maxRotationThisFrame;
	}

	setRotationAngle(rotationAngle + rotationAmount);
}

const Vector2& Transform::forward() const
{
	return forwardVector;
}

const Vector2& Transform::right() const
{
	return rightVector;
}

void Transform::updateDirectionVectors()
{
	// Convert rotation angle from degrees to radians
	float angleRad = rotationAngle * static_cast<float>(M_PI) / 180.0f;

	// Calculate forward vector (pointing up initially, rotated by angle)
	forwardVector = Vector2(std::sin(angleRad), std::cos(angleRad));

	// Right vector is forward rotated 90 degrees clockwise
	rightVector = Vector2(std::cos(angleRad), -std::sin(angleRad));
}

void Transform::serialize(WriteArchive& archive) const
{
	float posX = position.x;
	float posY = position.y;
	double rot = rotationAngle;
	float scaleX = scale.x;
	float scaleY = scale.y;

	archive.process(posX);
	archive.process(posY);
	archive.process(rot);
	archive.process(scaleX);
	archive.process(scaleY);
}

void Transform::deserialize(ReadArchive& archive)
{
	float posX, posY;
	double rot;
	float scaleX, scaleY;

	archive.process(posX);
	archive.process(posY);
	archive.process(rot);
	archive.process(scaleX);
	archive.process(scaleY);

	position = Vector2(posX, posY);
	rotationAngle = rot;
	scale = Vector2(scaleX, scaleY);

	updateDirectionVectors();
	markDirty();
}
