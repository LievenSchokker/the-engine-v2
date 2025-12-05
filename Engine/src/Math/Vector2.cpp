//
// Created by samle on 01/12/2025.
//


#include "Math/Vector2.h"

#include <cmath>
#include <algorithm>


constexpr float EPSILON = 1e-5f;

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


float Vector2::distance(const Vector2& from, const Vector2& to)
{
	float deltaX = to.x - from.x;
	float deltaY = to.y - from.y;

	return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}


float Vector2::dot(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}


float Vector2::angle(const Vector2& from, const Vector2& to)
{
	float fromMagnitude = from.magnitude();
	float toMagnitude = to.magnitude();

	/// Prevents divide by 0 situations.
	if (fromMagnitude <= 0.0f || toMagnitude <= 0.0f) return 0.0f;

	float dotProd = dot(from, to);

	float x = dotProd / (fromMagnitude * toMagnitude);

	/// Prevents domain error if x < -1 || > 1
	x = std::max(-1.0f, std::min(1.0f, x));

	return std::acos(x);
}


Vector2 Vector2::lerp(const Vector2& from, const Vector2& to, float t)
{
	t = std::clamp(t, 0.0f, 1.0f);
	return from + (to - from) * t;
}


void Vector2::normalize()
{
    float mag = magnitude();

    if (mag == 0.0f)
        return;

	x /= mag;
	y /= mag;
}

Vector2 Vector2::normalised() const
{
    float mag = magnitude();

    if (mag == 0.0f)
        return Vector2::zero();

    return Vector2{x / mag, y / mag};
}



float Vector2::magnitude() const
{
	return std::sqrt(x * x + y * y);
}


Vector2 Vector2::operator+(const Vector2& other) const
{
	return {x + other.x, y + other.y};
}


Vector2 Vector2::operator-(const Vector2& other) const
{
	return {x - other.x, y - other.y};
}


Vector2 Vector2::operator*(const Vector2& other) const
{
	return {x * other.x, y * other.y};
}


Vector2 Vector2::operator/(const Vector2& other) const
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


Vector2& Vector2::operator+=(const Vector2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}


Vector2& Vector2::operator-=(const Vector2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}


Vector2& Vector2::operator*=(const Vector2& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}


Vector2& Vector2::operator/=(const Vector2& other)
{
	x /= other.x;
	y /= other.y;
	return *this;
}


Vector2& Vector2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}


Vector2& Vector2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}

Vector2 Vector2::operator-() const
{
	return {-x, -y};
}


bool Vector2::operator==(const Vector2& other) const
{
	return std::fabs(x - other.x) < EPSILON && std::fabs(y - other.y) <
	       EPSILON;
}


bool Vector2::operator!=(const Vector2& other) const
{
	return !(*this == other);
}