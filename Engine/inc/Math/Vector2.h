#pragma once
#include <cmath>
/**
 * @brief A struct for storing 2D vector points.
 */
struct Vector2
{
    Vector2() = default;

    Vector2(const float x_, const float y_) : x(x_), y(y_) {}

    /// Shorthand for writing Vector2{1.0, 1.0}
    static Vector2 one();

    /// Shorthand for writing Vector2{0.0, 0.0}
    static Vector2 zero();

    /// Shorthand for writing Vector2{0.0, 1.0}
    static Vector2 up();

    /// Shorthand for writing Vector2{1.0, 0.0}
    static Vector2 right();

    /// Shorthand for writing Vector2{0.0, -1.0}
    static Vector2 down();

    /// Shorthand for writing Vector2{-1.0, 0.0}
    static Vector2 left();

    /// Returns the distance between two vectors.
    [[nodiscard]] static float distance(const Vector2 &from, const Vector2 &to);

    /// Returns the dot product between this and the other vector
    /// (1 point in the same direction; -1 complete opposite directions; 0 if perpendicular)
    [[nodiscard]] static float dot(const Vector2 &a, const Vector2 &b);

    /// Returns the angle between two vectors (in radians)
    [[nodiscard]] static float angle(const Vector2 &from, const Vector2 &to);

    /// Gets the value of the x component of this vector
    [[nodiscard]] float X() const;

    /// Gets the value of the y component of this vector
    [[nodiscard]] float Y() const;

    /// Sets the value of this vector's x component
    void setX(float value);

    /// Sets the value of this vector's y component
    void setY(float value);

    /// Makes this vector have a magnitude of 1, keeping the same directions.
    void normalize();

    /// Returns the length of this vector
    [[nodiscard]] float magnitude() const;

    /// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
    Vector2 operator+(const Vector2 &other) const;

    /// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
    Vector2 operator-(const Vector2 &other) const;

    /// Multiplies each element of the other Vector to this Vector's respective element (v1.x * v2.x, v1.y * v2.y)
    Vector2 operator*(const Vector2 &other) const;

    /// Divides each element of the other Vector to this Vector's respective element (v1.x / v2.x, v1.y / v2.y)
    Vector2 operator/(const Vector2 &other) const;

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2 operator*(float scalar) const;

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2 operator/(float scalar) const;

    /// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
    Vector2 &operator+=(const Vector2 &other);

    /// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
    Vector2 &operator-=(const Vector2 &other);

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2 &operator*=(const Vector2 &other);

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2 &operator/=(const Vector2 &other);

    /// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
    Vector2 &operator*=(float scalar);

    /// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
    Vector2 &operator/=(float scalar);

    private:
        float x = 0;
        float y = 0;
};
