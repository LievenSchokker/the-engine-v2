#include "Audio/AudioManager.h"
#include "FakeAudioBackend.h"

#include <gtest/gtest.h>

struct FakeAudioBackend;

TEST(AudioManagerTest, InitializeSuccess)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	AudioManager manager;

	EXPECT_TRUE(manager.initialize(std::move(fake)));
}

TEST(AudioManagerTest, LoadMusicCallsBackend)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* ptr = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	EXPECT_TRUE(mgr.loadMusic("song.ogg"));
	EXPECT_EQ(ptr->lastPlayMusic_handle, -1);  // loadMusic does not play
}

TEST(AudioManagerTest, PlaySound_UsesReserveChannelAndBackendPlay)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* ptr = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	SoundHandle handle = mgr.loadSound("hit.wav");
	EXPECT_EQ(handle, 10);

	int channel = mgr.playSound(handle, 0, 1.0f, 0.5f);

	EXPECT_EQ(channel, ptr->freeChannel);
	EXPECT_EQ(ptr->lastPlaySound_handle, 10);
	EXPECT_EQ(ptr->lastPlaySound_channel, ptr->freeChannel);
	EXPECT_EQ(ptr->lastPlaySound_loops, 0);
	EXPECT_EQ(ptr->lastPanLeft, 1.0f);
	EXPECT_EQ(ptr->lastPanRight, 0.5f);
}

TEST(AudioManagerTest, StopChannel_CallsBackend)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* ptr = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	mgr.stopChannel(7);
	EXPECT_EQ(ptr->lastStoppedChannel, 7);
}

TEST(AudioManagerTest, SetMusicVolume_ForwardsToBackend)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* ptr = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	mgr.setMusicVolume(0.45f);
	EXPECT_FLOAT_EQ(ptr->lastMusicVolume, 0.45f);
}
