#include "Audio/AudioManager.h"
#include "Audio/Components/MusicSource.h"
#include "FakeAudioBackend.h"

#include <gtest/gtest.h>

TEST(MusicSourceTest, LoadAndPlayMusic_CallsBackend)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* fakeAudioBackend = fake.get();
	AudioManager audioManager;
	audioManager.initialize(std::move(fake));

	MusicSource music(&audioManager);

	EXPECT_TRUE(music.loadMusic("bgm.ogg"));

	music.setLoop(true);
	music.setVolume(0.7f);
	music.play();

	EXPECT_EQ(fakeAudioBackend->lastPlayMusic_handle, 5);  // from fake loadMusic()
	EXPECT_EQ(fakeAudioBackend->lastPlayMusic_loops, 1);  // loop enabled
	EXPECT_FLOAT_EQ(fakeAudioBackend->lastMusicVolume, 0.7f);
}

TEST(MusicSourceTest, PauseResumeStop_CallsBackend)
{
	auto fake = std::make_unique<FakeAudioBackend>();
	FakeAudioBackend* ptr = fake.get();
	AudioManager mgr;
	mgr.initialize(std::move(fake));

	MusicSource music(&mgr);
	music.loadMusic("bgm.ogg");
	music.play();

	music.pause();
	music.resume();
	music.stop();

	EXPECT_EQ(ptr->lastPlayMusic_handle, 5);
}
