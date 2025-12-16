#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationTrack.h"
#include "Math/Vector2.h"

#include <gtest/gtest.h>

class AnimationClipTest: public ::testing::Test
{
   protected:
	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(AnimationClipTest, Constructor)
{
	AnimationClip clip("TestClip", false);

	EXPECT_EQ(clip.getName(), "TestClip");
	EXPECT_FALSE(clip.isLooping());
	EXPECT_FLOAT_EQ(clip.getLength(), 0.0f);
	EXPECT_EQ(clip.getTracks().size(), 0);
}

TEST_F(AnimationClipTest, Looping)
{
	AnimationClip clip("TestClip", true);
	EXPECT_TRUE(clip.isLooping());

	clip.setLoop(false);
	EXPECT_FALSE(clip.isLooping());
}

TEST_F(AnimationClipTest, SetName)
{
	AnimationClip clip("OldName", false);
	clip.setName("NewName");
	EXPECT_EQ(clip.getName(), "NewName");
}

TEST_F(AnimationClipTest, AddTrackUpdatesLength)
{
	AnimationClip clip("TestClip", false);

	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};

	AnimationTrack track1(TargetType::Transform, PropertyType::Position, 2.0f,
						  false, fromPos, toPos, curve);

	clip.addTrack(track1);
	EXPECT_FLOAT_EQ(clip.getLength(), 2.0f);
	EXPECT_EQ(clip.getTracks().size(), 1);

	AnimationTrack track2(TargetType::Transform, PropertyType::Position, 5.0f,
						  false, fromPos, toPos, curve);

	clip.addTrack(track2);
	// Tracks with the same property are sequential, so length is sum (2.0 + 5.0
	// = 7.0)
	EXPECT_FLOAT_EQ(clip.getLength(), 7.0f);
	EXPECT_EQ(clip.getTracks().size(), 2);
}

TEST_F(AnimationClipTest, GetTracks)
{
	AnimationClip clip("TestClip", false);

	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};

	AnimationTrack track(TargetType::Transform, PropertyType::Position, 1.0f,
						 false, fromPos, toPos, curve);

	clip.addTrack(track);

	const auto& tracks = clip.getTracks();
	EXPECT_EQ(tracks.size(), 1);
	EXPECT_FLOAT_EQ(tracks[0].getDuration(), 1.0f);
}
