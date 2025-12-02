#pragma once

#include <cmath>
/**
 * @brief A struct for storing 2D vector points.
 */
struct Vector2
{
    float x{};
    float y{};

    // Addition of two Vector2
    Vector2 operator+(const Vector2& other) const {
        return { x + other.x, y + other.y };
    }

    // Subtraction of two Vector2
    Vector2 operator-(const Vector2& other) const {
        return { x - other.x, y - other.y };
    }

    // Scalar multiplication
    Vector2 operator*(float scalar) const {
        return { x * scalar, y * scalar };
    }

    // Scalar division
    Vector2 operator/(float scalar) const {
        return { x / scalar, y / scalar };
    }

    // Compound assignment versions (optional but convenient)
    Vector2& operator+=(const Vector2& other) {
        x += other.x; y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x; y -= other.y;
        return *this;
    }

    Vector2& operator*=(float scalar) {
        x *= scalar; y *= scalar;
        return *this;
    }

    Vector2& operator/=(float scalar) {
        x /= scalar; y /= scalar;
        return *this;
    }
};