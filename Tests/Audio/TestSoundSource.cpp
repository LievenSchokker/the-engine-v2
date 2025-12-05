#include "Audio/AudioManager.h"
#include "Audio/Components/SoundSource.h"
#include "FakeAudioBackend.h"

#include <gtest/gtest.h>

TEST(SoundSourceTest, LoadAndPlaySound_UsesBackend)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* ptr = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	SoundSource src(&mgr);

	EXPECT_TRUE(src.loadSound("fx.wav"));

	src.setPanning(0.8f, 0.3f);
	src.play(1);

	EXPECT_EQ(ptr->lastPlaySound_handle, 10);
	EXPECT_EQ(ptr->lastPlaySound_loops, 1);
	EXPECT_EQ(ptr->lastPanLeft, 0.8f);
	EXPECT_EQ(ptr->lastPanRight, 0.3f);
}

TEST(SoundSourceTest, StopSound_CallsBackend)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* backend = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	SoundSource src(&mgr);
	src.loadSound("fx.wav");
	src.play();

	src.stop();

	EXPECT_EQ(backend->lastStoppedChannel, backend->freeChannel);
}

TEST(SoundSourceTest, SetCurrentPanning_AffectsExistingChannel)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* backend = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	SoundSource src(&mgr);
	src.loadSound("fx.wav");
	src.play();

	src.setCurrentPanning(0.2f, 1.0f);

	EXPECT_EQ(backend->lastPanLeft, 0.2f);
	EXPECT_EQ(backend->lastPanRight, 1.0f);
}
