#include "Animation/AnimationCurve.h"
#include <gtest/gtest.h>
#include <cmath>

class AnimationCurveTest : public ::testing::Test
{
protected:
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(AnimationCurveTest, LinearEasing)
{
	AnimationCurve curve(EasingType::Linear);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(0.5f), 0.5f);
	EXPECT_FLOAT_EQ(curve.evaluate01(1.0f), 1.0f);
}

TEST_F(AnimationCurveTest, ClampsToValidRange)
{
	AnimationCurve curve(EasingType::Linear);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(-1.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(2.0f), 1.0f);
}

TEST_F(AnimationCurveTest, EaseInQuad)
{
	AnimationCurve curve(EasingType::EaseInQuad);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(0.5f), 0.25f);
	EXPECT_FLOAT_EQ(curve.evaluate01(1.0f), 1.0f);
}

TEST_F(AnimationCurveTest, EaseOutQuad)
{
	AnimationCurve curve(EasingType::EaseOutQuad);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(0.5f), 0.75f);
	EXPECT_FLOAT_EQ(curve.evaluate01(1.0f), 1.0f);
}

TEST_F(AnimationCurveTest, EaseInOutQuad)
{
	AnimationCurve curve(EasingType::EaseInOutQuad);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(0.5f), 0.5f);
	EXPECT_FLOAT_EQ(curve.evaluate01(1.0f), 1.0f);
	
	// Should be slower at start and end
	EXPECT_LT(curve.evaluate01(0.25f), 0.25f);
	EXPECT_GT(curve.evaluate01(0.75f), 0.75f);
}

TEST_F(AnimationCurveTest, EaseInCubic)
{
	AnimationCurve curve(EasingType::EaseInCubic);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(0.5f), 0.125f);
	EXPECT_FLOAT_EQ(curve.evaluate01(1.0f), 1.0f);
}

TEST_F(AnimationCurveTest, EaseOutCubic)
{
	AnimationCurve curve(EasingType::EaseOutCubic);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(0.5f), 0.875f);
	EXPECT_FLOAT_EQ(curve.evaluate01(1.0f), 1.0f);
}

TEST_F(AnimationCurveTest, EaseInOutCubic)
{
	AnimationCurve curve(EasingType::EaseInOutCubic);
	
	EXPECT_FLOAT_EQ(curve.evaluate01(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(curve.evaluate01(0.5f), 0.5f);
	EXPECT_FLOAT_EQ(curve.evaluate01(1.0f), 1.0f);
}

TEST_F(AnimationCurveTest, GetSetType)
{
	AnimationCurve curve(EasingType::Linear);
	EXPECT_EQ(curve.getType(), EasingType::Linear);
	
	curve.setType(EasingType::EaseInQuad);
	EXPECT_EQ(curve.getType(), EasingType::EaseInQuad);
}

