#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationTrack.h"
#include "Animation/Animator.h"
#include "GameObject/GameObject.h"
#include "GameObject/Vector2.h"

#include <gtest/gtest.h>

class AnimatorTest: public ::testing::Test
{
   protected:
	void SetUp() override
	{
		gameObject = std::make_unique<GameObject>();
		gameObject->setName("TestObject");
		animator = gameObject->addComponent<Animator>();
	}

	void TearDown() override
	{
	}

	std::unique_ptr<GameObject> gameObject;
	Animator* animator;
};

TEST_F(AnimatorTest, Constructor)
{
	EXPECT_EQ(animator->getCurrentClip(), nullptr);
	EXPECT_FALSE(animator->getIsPlaying());
	EXPECT_FLOAT_EQ(animator->getTimeScale(), 1.0f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 0.0f);
}

TEST_F(AnimatorTest, Play)
{
	AnimationClip clip("TestClip", false);
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};

	AnimationTrack track(TargetType::Transform, PropertyType::Position, 1.0f,
						 false, fromPos, toPos, curve);
	clip.addTrack(track);

	animator->play(&clip);

	EXPECT_EQ(animator->getCurrentClip(), &clip);
	EXPECT_TRUE(animator->getIsPlaying());
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 0.0f);
}

TEST_F(AnimatorTest, Pause)
{
	AnimationClip clip("TestClip", false);
	animator->play(&clip);

	EXPECT_TRUE(animator->getIsPlaying());

	animator->pause();

	EXPECT_FALSE(animator->getIsPlaying());
	EXPECT_EQ(animator->getCurrentClip(), &clip);  // Clip should still be set
}

TEST_F(AnimatorTest, Stop)
{
	AnimationClip clip("TestClip", false);
	animator->play(&clip);
	animator->update(0.5f);	 // Advance time

	animator->stop();

	EXPECT_FALSE(animator->getIsPlaying());
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 0.0f);
}

TEST_F(AnimatorTest, TimeScale)
{
	animator->setTimeScale(2.0f);
	EXPECT_FLOAT_EQ(animator->getTimeScale(), 2.0f);

	animator->setTimeScale(0.5f);
	EXPECT_FLOAT_EQ(animator->getTimeScale(), 0.5f);
}

TEST_F(AnimatorTest, UpdateAdvancesTime)
{
	AnimationClip clip("TestClip", false);
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};

	AnimationTrack track(TargetType::Transform, PropertyType::Position, 2.0f,
						 false, fromPos, toPos, curve);
	clip.addTrack(track);

	animator->play(&clip);

	animator->update(0.5f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 0.5f);

	animator->update(0.5f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 1.0f);
}

TEST_F(AnimatorTest, TimeScaleAffectsUpdate)
{
	AnimationClip clip("TestClip", false);
	animator->play(&clip);
	animator->setTimeScale(2.0f);

	animator->update(0.5f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(),
					1.0f);	// Should advance 2x faster
}

TEST_F(AnimatorTest, LoopingClip)
{
	AnimationClip clip("TestClip", true);
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};

	AnimationTrack track(TargetType::Transform, PropertyType::Position, 1.0f,
						 false, fromPos, toPos, curve);
	clip.addTrack(track);

	animator->play(&clip);
	animator->update(1.5f);	 // Go past end

	// Should loop back
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 0.5f);
	EXPECT_TRUE(animator->getIsPlaying());
}

TEST_F(AnimatorTest, NonLoopingClipStops)
{
	AnimationClip clip("TestClip", false);
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};

	AnimationTrack track(TargetType::Transform, PropertyType::Position, 1.0f,
						 false, fromPos, toPos, curve);
	clip.addTrack(track);

	animator->play(&clip);
	animator->update(1.5f);	 // Go past end

	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 1.0f);	// Clamped to length
	EXPECT_FALSE(animator->getIsPlaying());				// Should stop
}

TEST_F(AnimatorTest, GetGameObject)
{
	EXPECT_EQ(animator->getGameObject(), gameObject.get());
}
