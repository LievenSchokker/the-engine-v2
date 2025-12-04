//
// Created by samle on 01/12/2025.
//


#include "Math/Vector2.h"

#include <cmath>
#include <algorithm>


constexpr float EPSILON = 1e-5f;


float Vector2::x() const
{
    return _x;
}


float Vector2::y() const
{
    return _y;
}


void Vector2::setX(float value)
{
    _x = value;
}


void Vector2::setY(float value)
{
    _y = value;
}


Vector2 Vector2::one()
{
    return Vector2{1.0f, 1.0f};
}


Vector2 Vector2::zero()
{
    return Vector2{0.0f, 0.0f};
}


Vector2 Vector2::up()
{
    return Vector2{0.0f, 1.0f};
}


Vector2 Vector2::right()
{
    return Vector2{1.0f, 0.0f};
}


Vector2 Vector2::down()
{
    return Vector2{0.0f, -1.0f};
}


Vector2 Vector2::left()
{
    return Vector2{-1.0f, 0.0f};
}


float Vector2::distance(const Vector2 &from, const Vector2 &to)
{
    float deltaX = to.x() - from.x();
    float deltaY = to.y() - from.y();

    return std::sqrt(deltaX*deltaX + deltaY*deltaY);
}


float Vector2::dot(const Vector2 &a, const Vector2 &b)
{
    return a.x() * b.x() + a.y() * b.y();

}


float Vector2::angle(const Vector2 &from, const Vector2 &to)
{
    float fromMagnitude = from.magnitude();
    float toMagnitude = to.magnitude();

    /// Prevents divide by 0 situations.
    if (fromMagnitude <= 0.0f || toMagnitude <= 0.0f)
        return 0.0f;

    float dotProd = dot(from, to);

    float x = dotProd / (fromMagnitude * toMagnitude);

    /// Prevents domain error if x < -1 || > 1
    x = std::max(-1.0f, std::min(1.0f, x));

    return std::acos(x);
}


Vector2 Vector2::lerp(const Vector2 &from, const Vector2 &to, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return from + (to - from) * t;
}


void Vector2::normalize()
{
    float mag = magnitude();
    if (mag == 0.0f) return;

    _x /= mag;
    _y /= mag;
}


float Vector2::magnitude() const
{
    return std::sqrt(_x * _x + _y * _y);
}


Vector2 Vector2::operator+(const Vector2 &other) const
{
    return {_x + other._x, _y + other._y};
}


Vector2 Vector2::operator-(const Vector2 &other) const
{
    return {_x - other._x, _y - other._y};
}


Vector2 Vector2::operator*(const Vector2 &other) const
{
    return {_x * other._x, _y * other._y};
}


Vector2 Vector2::operator/(const Vector2 &other) const
{
    return {_x / other._x, _y / other._y};
}


Vector2 Vector2::operator*(float scalar) const
{
    return {_x * scalar, _y * scalar};
}


Vector2 Vector2::operator/(float scalar) const
{
    return {_x / scalar, _y / scalar};
}


Vector2 &Vector2::operator+=(const Vector2 &other)
{
    _x += other._x;
    _y += other._y;
    return *this;
}


Vector2 &Vector2::operator-=(const Vector2 &other)
{
    _x -= other._x;
    _y -= other._y;
    return *this;
}


Vector2 &Vector2::operator*=(const Vector2 &other)
{
    _x *= other._x;
    _y *= other._y;
    return *this;
}


Vector2 &Vector2::operator/=(const Vector2 &other)
{
    _x /= other._x;
    _y /= other._y;
    return *this;
}


Vector2 &Vector2::operator*=(float scalar)
{
    _x *= scalar;
    _y *= scalar;
    return *this;
}


Vector2 &Vector2::operator/=(float scalar)
{
    _x /= scalar;
    _y /= scalar;
    return *this;
}

Vector2 Vector2::operator-() const
{
    return {-_x, -_y};
}


bool Vector2::operator==(const Vector2& other) const
{
    return std::fabs(_x - other._x) < EPSILON && std::fabs(_y - other._y) < EPSILON;
}


bool Vector2::operator!=(const Vector2& other) const
{
    return !(*this == other);
}