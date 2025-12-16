#pragma once


#include "GameObject/GameObject.h"
#include "Math/Vector2.h"
#include "Component/BaseComponentTypes/Component.h"

/**
 * @brief Transform holds the position, rotation and scale.
 */
class Transform: public Component
{
   public:
	Transform(Vector2 position = {0.0, 0.0}, double rotationAngle = 0,
			  Vector2 scale = {1.0, 1.0});
	~Transform();

    /**
     * @brief Get the current position.
     *
     * @return A Vector2 of the current position.
     */
    Vector2 getPosition() const;
    /**
     * @brief Get the current rotation angle.
     *
     * @return A double of the current rotation angle.
     */
    float getRotationAngle() const;
    /**
     * @brief Get the current scale.
     *
     * @return A Vector2 of the current scale size.
     */
    Vector2 getScale() const;

    /**
    * @brief Set the new position.
    *
    * @param newPosition Vector2 of the new position.
    */
    void setPosition(Vector2 newPosition);
    /**
    * @brief Set the new rotation angle.
    *
    * @param newRotationAngle Double of the new rotation angle.
    */
    void setRotationAngle(double newRotationAngle);
    /**
    * @brief Set the new scale.
    *
    * @param newScale Vector2 of the new scale.
    */
    void setScale(Vector2 newScale);

    /**
     * @brief  Moves the Transform towards a target position.
     *
     *
     * @param targetPosition the position to move towards
     * @param maxDistance Maximum allowed distance the Transform can move in this call.
     * If the targetPosition is closer than the maxDistance, the transform will snap to it.
     */
    void moveTowards(Vector2 targetPosition, float maxDistance);


    void rotateTowards(const Vector2& targetDirection, float maxRotationSpeed, float deltaTime);

    const Vector2& forward() const;
    const Vector2& right() const;


	ComponentType getComponentType() const override
	{
		return ComponentType::Transform;
	}

	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;
private:
    void updateDirectionVectors();
    Vector2 position;
    float rotationAngle;
    Vector2 scale;
    Vector2 forwardVector = Vector2(0, 1);
    Vector2 rightVector = Vector2(1, 0);
};
