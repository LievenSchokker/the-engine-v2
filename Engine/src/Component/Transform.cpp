#include "Component/Transform.h"

#include "GameObject/GameObject.h"
#include "Math/Matrix3.h"

Transform::Transform(const Vector2 position, const double rotationAngle,
					 const Vector2 scale)
	: position(position),
	  rotationAngle(rotationAngle),
	  scale(scale),
	  parent(nullptr),
	  hasEverHadParent(false),
	  cachedLocalMatrix(Matrix3::identity()),
	  cachedWorldMatrix(Matrix3::identity()),
	  isLocalMatrixDirty(true),
	  isWorldMatrixDirty(true)
{
}

Transform::~Transform() = default;

Vector2 Transform::getPosition() const
{
	return position;
}

double Transform::getRotationAngle() const
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
	isLocalMatrixDirty = true;
	isWorldMatrixDirty = true;
	// Mark all children as dirty too (their world matrices depend on this
	// transform)
	if ( gameObject != nullptr )
	{
		const std::vector<GameObject*>& children = gameObject->getChildren();
		for ( GameObject* child : children )
		{
			if ( child != nullptr && child->getTransform() != nullptr )
			{
				child->getTransform()->markDirty();
			}
		}
	}
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
