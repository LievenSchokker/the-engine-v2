#pragma once

#include <cmath>

/**
 * @brief A struct for storing 2D vector points.
 */
struct Vector2
{
    /// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
    Vector2 operator+(const Vector2& other) const;

    /// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
    Vector2 operator-(const Vector2& other) const;

    /// Multiplies each element of the other Vector to this Vector's respective element (v1.x * v2.x, v1.y * v2.y)
    Vector2 operator*(const Vector2& other) const;

    /// Divides each element of the other Vector to this Vector's respective element (v1.x / v2.x, v1.y / v2.y)
    Vector2 operator/(const Vector2& other) const;

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2 operator*(float scalar) const;

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2 operator/(float scalar) const;

    /// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
    Vector2& operator+=(const Vector2& other);

    /// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
    Vector2& operator-=(const Vector2& other);

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2& operator*=(const Vector2& other);

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2& operator/=(const Vector2& other);

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2& operator*=(float scalar);

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2& operator/=(float scalar);

    /// Makes this vector have a magnitude of 1, keeping the same directions.
    void normalize();

    /// Returns the length of this vector
   [[nodiscard]] float magnitude() const;

    /// Returns the distance between two vectors.
   [[nodiscard]] static float distance(const Vector2& from, const Vector2& to);

    /// Returns the dot product between this and the other vector
    /// (1 point in the same direction; -1 complete opposite directions; 0 if perpendicular)
    [[nodiscard]] static float dot(const Vector2& a, const Vector2& b);

    /// Returns the angle between two vectors.
    [[nodiscard]] static float angle(const Vector2& from, const Vector2& to);

    [[nodiscard]] float X() const;
    [[nodiscard]] float Y() const;

    void setX(float value);
    void setY(float value);

    private:
        float x = 0;
        float y = 0;
};