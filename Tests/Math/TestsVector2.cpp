//
// Created by samle on 01/12/2025.
//

#include "gtest/gtest.h"
#include "Math/Vector2.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



constexpr float EPSILON = 1e-5f;

bool FloatEquals(float a, float b)
{
    return std::fabs(a - b) < EPSILON;
}



TEST(Vector2Test, Constructor)
{
    Vector2 v(3.5f, -2.5f);
    EXPECT_TRUE(FloatEquals(v.x(), 3.5f));
    EXPECT_TRUE(FloatEquals(v.y(), -2.5f));
}


TEST(Vector2Test, Setters)
{
    Vector2 v = Vector2::zero();
    v.setX(10.0f);
    v.setY(-5.0f);
    EXPECT_TRUE(FloatEquals(v.x(), 10.0f));
    EXPECT_TRUE(FloatEquals(v.y(), -5.0f));
}


TEST(Vector2Test, StaticVectors)
{
    Vector2 one = Vector2::one();
    EXPECT_TRUE(FloatEquals(one.x(), 1.0f) && FloatEquals(one.y(), 1.0f));

    Vector2 zero = Vector2::zero();
    EXPECT_TRUE(FloatEquals(zero.x(), 0.0f) && FloatEquals(zero.y(), 0.0f));

    EXPECT_TRUE(FloatEquals(Vector2::up().x(), 0.0f) && FloatEquals(Vector2::up().y(), 1.0f));
    EXPECT_TRUE(FloatEquals(Vector2::down().x(), 0.0f) && FloatEquals(Vector2::down().y(), -1.0f));
    EXPECT_TRUE(FloatEquals(Vector2::right().x(), 1.0f) && FloatEquals(Vector2::right().y(), 0.0f));
    EXPECT_TRUE(FloatEquals(Vector2::left().x(), -1.0f) && FloatEquals(Vector2::left().y(), 0.0f));
}


TEST(Vector2Test, OperatorAddSubtract)
{
    Vector2 a(1, 2);
    Vector2 b(3, 4);

    Vector2 sum = a + b;
    EXPECT_TRUE(FloatEquals(sum.x(), 4.0f));
    EXPECT_TRUE(FloatEquals(sum.y(), 6.0f));

    Vector2 diff = b - a;
    EXPECT_TRUE(FloatEquals(diff.x(), 2.0f));
    EXPECT_TRUE(FloatEquals(diff.y(), 2.0f));
}


TEST(Vector2Test, OperatorMultiplyDivide)
{
    Vector2 a(2, 3);
    Vector2 b(4, 5);

    Vector2 prod = a * b;
    EXPECT_TRUE(FloatEquals(prod.x(), 8.0f));
    EXPECT_TRUE(FloatEquals(prod.y(), 15.0f));

    Vector2 quot = b / a;
    EXPECT_TRUE(FloatEquals(quot.x(), 2.0f));
    EXPECT_TRUE(FloatEquals(quot.y(), 5.0f/3.0f));
}


TEST(Vector2Test, ScalarMultiplyDivide)
{
    Vector2 v(3, 6);
    float s = 2.0f;

    Vector2 mult = v * s;
    EXPECT_TRUE(FloatEquals(mult.x(), 6.0f));
    EXPECT_TRUE(FloatEquals(mult.y(), 12.0f));

    Vector2 div = v / s;
    EXPECT_TRUE(FloatEquals(div.x(), 1.5f));
    EXPECT_TRUE(FloatEquals(div.y(), 3.0f));
}


TEST(Vector2Test, CompoundOperators)
{
    Vector2 v(2, 3);
    Vector2 u(1, 1);

    v += u;
    EXPECT_TRUE(FloatEquals(v.x(), 3.0f));
    EXPECT_TRUE(FloatEquals(v.y(), 4.0f));

    v -= u;
    EXPECT_TRUE(FloatEquals(v.x(), 2.0f));
    EXPECT_TRUE(FloatEquals(v.y(), 3.0f));

    v *= u;
    EXPECT_TRUE(FloatEquals(v.x(), 2.0f));
    EXPECT_TRUE(FloatEquals(v.y(), 3.0f));

    v /= u;
    EXPECT_TRUE(FloatEquals(v.x(), 2.0f));
    EXPECT_TRUE(FloatEquals(v.y(), 3.0f));

    v *= 2.0f;
    EXPECT_TRUE(FloatEquals(v.x(), 4.0f));
    EXPECT_TRUE(FloatEquals(v.y(), 6.0f));

    v /= 2.0f;
    EXPECT_TRUE(FloatEquals(v.x(), 2.0f));
    EXPECT_TRUE(FloatEquals(v.y(), 3.0f));
}


TEST(Vector2Test, Magnitude)
{
    Vector2 v(3, 4);
    EXPECT_TRUE(FloatEquals(v.magnitude(), 5.0f));

    Vector2 zero = Vector2::zero();
    EXPECT_TRUE(FloatEquals(zero.magnitude(), 0.0f));
}

TEST(Vector2Test, Normalize)
{
    Vector2 v(3, 4);
    v.normalize();
    EXPECT_TRUE(FloatEquals(v.magnitude(), 1.0f));

    Vector2 zero = Vector2::zero();
    zero.normalize();
    EXPECT_TRUE(FloatEquals(zero.magnitude(), 0.0f));
}


TEST(Vector2Test, DotProduct)
{
    Vector2 a(1, 0);
    Vector2 b(0, 1);
    Vector2 c(1, 0);

    /// perpendicular:
    EXPECT_TRUE(FloatEquals(Vector2::dot(a, b), 0.0f));

    // same direction:
    EXPECT_TRUE(FloatEquals(Vector2::dot(a, c), 1.0f));

    // opposite:
    EXPECT_TRUE(FloatEquals(Vector2::dot(a, -c), -1.0f));
}


TEST(Vector2Test, Distance)
{
    Vector2 a(0, 0);
    Vector2 b(3, 4);

    EXPECT_TRUE(FloatEquals(Vector2::distance(a, b), 5.0f));
}


TEST(Vector2Test, Angle)
{
    Vector2 a(1, 0);
    Vector2 b(0, 1);
    Vector2 c(1, 0);
    Vector2 d(-1, 0);

    EXPECT_TRUE(FloatEquals(Vector2::angle(a, b), M_PI / 2)); // 90 degrees
    EXPECT_TRUE(FloatEquals(Vector2::angle(a, c), 0.0f));     // 0 degrees
    EXPECT_TRUE(FloatEquals(Vector2::angle(a, d), M_PI));     // 180 degrees
}


TEST(Vector2Test, Lerp)
{
    Vector2 a(0, 0);
    Vector2 b(10, 20);

    Vector2 mid = Vector2::lerp(a, b, 0.5f);
    EXPECT_TRUE(FloatEquals(mid.x(), 5.0f));
    EXPECT_TRUE(FloatEquals(mid.y(), 10.0f));

    Vector2 start = Vector2::lerp(a, b, 0.0f);
    EXPECT_TRUE(FloatEquals(start.x(), 0.0f));
    EXPECT_TRUE(FloatEquals(start.y(), 0.0f));

    Vector2 end = Vector2::lerp(a, b, 1.0f);
    EXPECT_TRUE(FloatEquals(end.x(), 10.0f));
    EXPECT_TRUE(FloatEquals(end.y(), 20.0f));

    // t > 1 is clamped
    Vector2 beyond = Vector2::lerp(a, b, 2.0f);
    EXPECT_TRUE(FloatEquals(beyond.x(), 10.0f));
    EXPECT_TRUE(FloatEquals(beyond.y(), 20.0f));

    // t < 0 is clamped
    Vector2 before = Vector2::lerp(a, b, -1.0f);
    EXPECT_TRUE(FloatEquals(before.x(), 0.0f));
    EXPECT_TRUE(FloatEquals(before.y(), 0.0f));
}
