#pragma once

#include <cmath>

/**
 * @brief A struct for storing 2D vector points.
 */
struct Vector2
{
    float x{};
    float y{};

    /// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
    Vector2 operator+(const Vector2& other) const
    {
        return { x + other.x, y + other.y };
    }

    /// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
    Vector2 operator-(const Vector2& other) const
    {
        return { x - other.x, y - other.y };
    }

    /// Multiplies each element of the other Vector to this Vector's respective element (v1.x * v2.x, v1.y * v2.y)
    Vector2 operator*(const Vector2& other) const
    {
        return { x * other.x, y * other.y };
    }

    /// Divides each element of the other Vector to this Vector's respective element (v1.x / v2.x, v1.y / v2.y)
    Vector2 operator/(const Vector2& other) const
    {
        return { x / other.x, y / other.y };
    }


    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2 operator*(float scalar) const
    {
        return { x * scalar, y * scalar };
    }

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2 operator/(float scalar) const
    {
        return { x / scalar, y / scalar };
    }

    /// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
    Vector2& operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    /// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
    Vector2& operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2& operator*=(const Vector2& other)
    {
        x *= other.x;
        y *= other.y;

        return *this;
    }

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2& operator/=(const Vector2& other)
    {
        x /= other.x;
        y /= other.y;

        return *this;
    }

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;

        return *this;
    }

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;

        return *this;
    }
};