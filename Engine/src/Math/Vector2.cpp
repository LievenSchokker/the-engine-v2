//
// Created by samle on 01/12/2025.
//
#pragma once

#include "Math/Vector2.h"
#include <cmath>


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
    float dx = to.X() - from.X();
    float dy = to.Y() - from.Y();

    return std::sqrt(dx*dx + dy*dy);
}


float Vector2::dot(const Vector2 &a, const Vector2 &b)
{
    return a.X() * b.X() + a.Y() * b.Y();

}


float Vector2::angle(const Vector2 &from, const Vector2 &to)
{
    float dotProd = dot(from, to);
    float magProd = from.magnitude() * to.magnitude();

    return std::acos(dotProd / magProd);
}


void Vector2::normalize()
{
    float mag = magnitude();
    if (mag == 0.0f) return;

    x /= mag;
    y /= mag;
}


float Vector2::magnitude() const
{
    return std::sqrt(x * x + y * y);
}


Vector2 Vector2::operator+(const Vector2 &other) const
{
    return {x + other.x, y + other.y};
}


Vector2 Vector2::operator-(const Vector2 &other) const
{
    return {x - other.x, y - other.y};
}


Vector2 Vector2::operator*(const Vector2 &other) const
{
    return {x * other.x, y * other.Y()};
}


Vector2 Vector2::operator/(const Vector2 &other) const
{
    return {x / other.x, y / other.y};
}


Vector2 Vector2::operator*(float scalar) const
{
    return {x * scalar, y * scalar};
}


Vector2 Vector2::operator/(float scalar) const
{
    return {x / scalar, y / scalar};
}


Vector2 &Vector2::operator+=(const Vector2 &other)
{
    x += other.x;
    y += other.y;
    return *this;
}


Vector2 &Vector2::operator-=(const Vector2 &other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}


Vector2 &Vector2::operator*=(const Vector2 &other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}


Vector2 &Vector2::operator/=(const Vector2 &other)
{
    x /= other.x;
    y /= other.y;
    return *this;
}


Vector2 &Vector2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}


Vector2 &Vector2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}