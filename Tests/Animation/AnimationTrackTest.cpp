#include "Animation/AnimationTrack.h"
#include "Animation/AnimationCurve.h"
#include "GameObject/GameObject.h"
#include "GameObject/Vector2.h"
#include "Component/Transform.h"
#include <gtest/gtest.h>

class AnimationTrackTest : public ::testing::Test
{
protected:
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(AnimationTrackTest, Constructor)
{
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};
	
	AnimationTrack track(TargetType::Transform,
	                     PropertyType::Position,
	                     1.0f,
	                     false,
	                     fromPos,
	                     toPos,
	                     curve);
	
	EXPECT_EQ(track.getTarget(), TargetType::Transform);
	EXPECT_EQ(track.getProperty(), PropertyType::Position);
	EXPECT_FLOAT_EQ(track.getDuration(), 1.0f);
	EXPECT_FALSE(track.isRelative());
}

TEST_F(AnimationTrackTest, SampleVector2)
{
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 200.0f};
	
	AnimationTrack track(TargetType::Transform,
	                     PropertyType::Position,
	                     1.0f,
	                     false,
	                     fromPos,
	                     toPos,
	                     curve);
	
	auto result0 = track.sample(0.0f);
	ASSERT_TRUE(std::holds_alternative<Vector2>(result0));
	Vector2 val0 = std::get<Vector2>(result0);
	EXPECT_FLOAT_EQ(val0.x, 0.0f);
	EXPECT_FLOAT_EQ(val0.y, 0.0f);
	
	auto result1 = track.sample(1.0f);
	ASSERT_TRUE(std::holds_alternative<Vector2>(result1));
	Vector2 val1 = std::get<Vector2>(result1);
	EXPECT_FLOAT_EQ(val1.x, 100.0f);
	EXPECT_FLOAT_EQ(val1.y, 200.0f);
	
	auto resultMid = track.sample(0.5f);
	ASSERT_TRUE(std::holds_alternative<Vector2>(resultMid));
	Vector2 valMid = std::get<Vector2>(resultMid);
	EXPECT_FLOAT_EQ(valMid.x, 50.0f);
	EXPECT_FLOAT_EQ(valMid.y, 100.0f);
}

TEST_F(AnimationTrackTest, SampleFloat)
{
	AnimationCurve curve(EasingType::Linear);
	float fromRot = 0.0f;
	float toRot = 90.0f;
	
	AnimationTrack track(TargetType::Transform,
	                     PropertyType::Rotation,
	                     1.0f,
	                     false,
	                     fromRot,
	                     toRot,
	                     curve);
	
	auto result0 = track.sample(0.0f);
	ASSERT_TRUE(std::holds_alternative<float>(result0));
	EXPECT_FLOAT_EQ(std::get<float>(result0), 0.0f);
	
	auto result1 = track.sample(1.0f);
	ASSERT_TRUE(std::holds_alternative<float>(result1));
	EXPECT_FLOAT_EQ(std::get<float>(result1), 90.0f);
	
	auto resultMid = track.sample(0.5f);
	ASSERT_TRUE(std::holds_alternative<float>(resultMid));
	EXPECT_FLOAT_EQ(std::get<float>(resultMid), 45.0f);
}

TEST_F(AnimationTrackTest, ApplyPositionAbsolute)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 200.0f};
	
	AnimationTrack track(TargetType::Transform,
	                     PropertyType::Position,
	                     1.0f,
	                     false,
	                     fromPos,
	                     toPos,
	                     curve);
	
	track.apply(go.get(), 0.0f);
	Vector2 pos0 = go->getTransform()->getPosition();
	EXPECT_FLOAT_EQ(pos0.x, 0.0f);
	EXPECT_FLOAT_EQ(pos0.y, 0.0f);
	
	track.apply(go.get(), 1.0f);
	Vector2 pos1 = go->getTransform()->getPosition();
	EXPECT_FLOAT_EQ(pos1.x, 100.0f);
	EXPECT_FLOAT_EQ(pos1.y, 200.0f);
}

TEST_F(AnimationTrackTest, ApplyRotationAbsolute)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	
	AnimationCurve curve(EasingType::Linear);
	float fromRot = 0.0f;
	float toRot = 90.0f;
	
	AnimationTrack track(TargetType::Transform,
	                     PropertyType::Rotation,
	                     1.0f,
	                     false,
	                     fromRot,
	                     toRot,
	                     curve);
	
	track.apply(go.get(), 0.0f);
	EXPECT_FLOAT_EQ(go->getTransform()->getRotationAngle(), 0.0);
	
	track.apply(go.get(), 1.0f);
	EXPECT_FLOAT_EQ(go->getTransform()->getRotationAngle(), 90.0);
}

TEST_F(AnimationTrackTest, ApplyScaleAbsolute)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromScale{1.0f, 1.0f};
	Vector2 toScale{2.0f, 3.0f};
	
	AnimationTrack track(TargetType::Transform,
	                     PropertyType::Scale,
	                     1.0f,
	                     false,
	                     fromScale,
	                     toScale,
	                     curve);
	
	track.apply(go.get(), 0.0f);
	Vector2 scale0 = go->getTransform()->getScale();
	EXPECT_FLOAT_EQ(scale0.x, 1.0f);
	EXPECT_FLOAT_EQ(scale0.y, 1.0f);
	
	track.apply(go.get(), 1.0f);
	Vector2 scale1 = go->getTransform()->getScale();
	EXPECT_FLOAT_EQ(scale1.x, 2.0f);
	EXPECT_FLOAT_EQ(scale1.y, 3.0f);
}

