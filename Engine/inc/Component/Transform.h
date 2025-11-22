#pragma once


#include "../GameObject/GameObject.h"
#include "../GameObject/Vector2.h"
#include "Component.h"

/**
 * @brief Transform holds the position, rotation and scale.
 */
class Transform : public Component
{
public:
    Transform(Vector2 position = {0.0, 0.0}, double rotationAngle = 0, Vector2 scale = {0.0, 0.0});
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
    double getRotationAngle() const;
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
private:
    Vector2 position;
    double rotationAngle;
    Vector2 scale;
};