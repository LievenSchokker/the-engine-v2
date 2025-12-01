//
// Created by samle on 01/12/2025.
//
#pragma once
#include "Math/Vector2.h"
/// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
Vector2 Vector2::operator+(const Vector2 &other) const
{
    return {x + other.x, y + other.y};
}

/// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
Vector2 Vector2::operator-(const Vector2 &other) const
{
    return {x - other.x, y - other.y};
}

/// Multiplies each element of the other Vector to this Vector's respective element (v1.x * v2.x, v1.y * v2.y)
Vector2 Vector2::operator*(const Vector2 &other) const
{
    return {x * other.x, y * other.Y()};
}

/// Divides each element of the other Vector to this Vector's respective element (v1.x / v2.x, v1.y / v2.y)
Vector2 Vector2::operator/(const Vector2 &other) const
{
    return {x / other.x, y / other.y};
}

/// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
Vector2 Vector2::operator*(float scalar) const
{
    return {x * scalar, y * scalar};
}

/// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
Vector2 Vector2::operator/(float scalar) const
{
    return {x / scalar, y / scalar};
}

/// Adds each element of the other Vector to this Vector's respective element (v1.x + v2.x, v1.y + v2.y)
Vector2 &Vector2::operator+=(const Vector2 &other)
{
    x += other.x;
    y += other.y;
    return *this;
}

/// Subtracts each element of the other Vector to this Vector's respective element (v1.x - v2.x, v1.y - v2.y)
Vector2 &Vector2::operator-=(const Vector2 &other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

/// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
Vector2 &Vector2::operator*=(const Vector2 &other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}

/// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
Vector2 &Vector2::operator/=(const Vector2 &other)
{
    x /= other.x;
    y /= other.y;
    return *this;
}

/// Multiplies this vector's component by a scalar value (v1.x * scalar, v1.y * scalar)
Vector2 &Vector2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

/// Divides this vector's component by a scalar value (v1.x / scalar, v1.y / scalar)
Vector2 &Vector2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}

float Vector2::X() const
{
    return x;
}

float Vector2::Y() const
{
    return y;
}

void Vector2::setX(float value)
{
    x = value;
}

void Vector2::setY(float value)
{
    y = value;
}

void Vector2::normalize()
{

}

