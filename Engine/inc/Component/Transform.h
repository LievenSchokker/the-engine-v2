#pragma once

#include "Component/BaseComponentTypes/Component.h"
#include "GameObject/GameObject.h"
#include "Math/Matrix3.h"
#include "Math/Vector2.h"
#include "Networking/Serialization/RegistrationBase.h"

/**
 * @brief Transform holds the position, rotation and scale.
 * Supports parent-child hierarchy through matrix multiplication.
 */
class Transform: public Component, RegistrationBase<Transform>
{
   public:
	Transform(Vector2 position = {0.0, 0.0}, double rotationAngle = 0,
			  Vector2 scale = {1.0, 1.0});
	~Transform();

	static constexpr const char* name()
	{
		return "Transform";
	}

	const char* getName() const override
	{
		return name();
	}

	/**
	 * @brief Get the current local position.
	 *
	 * @return A Vector2 of the current local position.
	 */
	Vector2 getPosition() const;
	/**
	 * @brief Get the current local rotation angle.
	 *
	 * @return A float of the current rotation angle.
	 */
	float getRotationAngle() const;
	/**
	 * @brief Get the current local scale.
	 *
	 * @return A Vector2 of the current scale size.
	 */
	Vector2 getScale() const;

	/**
	 * @brief Set the new local position.
	 *
	 * @param newPosition Vector2 of the new position.
	 */
	void setPosition(Vector2 newPosition);
	/**
	 * @brief Set the new local rotation angle.
	 *
	 * @param newRotationAngle Double of the new rotation angle.
	 */
	void setRotationAngle(double newRotationAngle);
	/**
	 * @brief Set the new local scale.
	 *
	 * @param newScale Vector2 of the new scale.
	 */
	void setScale(Vector2 newScale);

	/**
	 * @brief Get the world position (local position transformed by parent
	 * hierarchy).
	 * @return A Vector2 of the world position.
	 */
	Vector2 getWorldPosition() const;

	/**
	 * @brief Get the world rotation angle (local rotation + parent rotations).
	 * @return A double of the world rotation angle in degrees.
	 */
	double getWorldRotation() const;

	/**
	 * @brief Get the world scale (local scale multiplied by parent scales).
	 * @return A Vector2 of the world scale.
	 */
	Vector2 getWorldScale() const;

	/**
	 * @brief Get the local transformation matrix.
	 * @return The local transformation matrix.
	 */
	Matrix3 getLocalMatrix() const;

	/**
	 * @brief Get the world transformation matrix (local * parent world matrix).
	 * @return The world transformation matrix.
	 */
	Matrix3 getWorldMatrix() const;

	/**
	 * @brief Set the parent transform.
	 * @param parent The parent transform, or nullptr to remove parent.
	 * @return true if the parent change was applied, false if rejected
	 * (e.g. circular relationship).
	 */
	bool setParent(Transform* parent);

	/**
	 * @brief Get the parent transform.
	 * @return Pointer to the parent transform, or nullptr if no parent.
	 */
	Transform* getParent() const;

	/**
	 * @brief Mark this transform and all children as dirty (forces matrix
	 * recomputation).
	 *
	 * Delegates to GameObject to handle child propagation.
	 */
	void markDirty();

	/**
	 * @brief Mark only this transform as dirty (without propagating to
	 * children).
	 *
	 * Used internally by GameObject::markTransformDirty() to mark individual
	 * transforms.
	 */
	void markDirtyLocal();

	/**
	 * @brief  Moves the Transform towards a target position.
	 *
	 *
	 * @param targetPosition the position to move towards
	 * @param maxDistance Maximum allowed distance the Transform can move in
	 * this call. If the targetPosition is closer than the maxDistance, the
	 * transform will snap to it.
	 */
	void moveTowards(Vector2 targetPosition, float maxDistance);

	/**
	 * @brief Rotates the Transform towards a target direction.
	 *
	 * @param targetDirection The direction to rotate towards
	 * @param maxRotationSpeed Maximum rotation speed in degrees per second
	 * @param deltaTime Time elapsed since last frame
	 */
	void rotateTowards(const Vector2& targetDirection, float maxRotationSpeed,
					   float deltaTime);

	const Vector2& forward() const;
	const Vector2& right() const;
	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;

   private:
	void updateDirectionVectors();
	Vector2 position;
	float rotationAngle;
	Vector2 scale;

	Transform* parent;
	bool hasEverHadParent;	// Track if we've ever had a parent (to distinguish
							// first attachment from re-attachment)
	mutable Matrix3 cachedLocalMatrix;
	mutable Matrix3 cachedWorldMatrix;
	mutable bool isLocalMatrixDirty;
	mutable bool isWorldMatrixDirty;

	Vector2 forwardVector;
	Vector2 rightVector;

	void updateLocalMatrix() const;
	void updateWorldMatrix() const;
};
