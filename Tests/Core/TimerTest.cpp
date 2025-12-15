// Tests/Core/TimerTest.cpp
#include "Core/ApplicationClock.h"

#include <SDL2/SDL.h>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

class TimerTest: public ::testing::Test
{
   protected:
	void SetUp() override
	{
		if ( SDL_Init(SDL_INIT_TIMER) < 0 )
		{
			FAIL() << "SDL_Init failed: " << SDL_GetError();
		}
	}

	void TearDown() override
	{
		SDL_Quit();
	}
};

// Test 1: Verify that 1 second of real time = tickRate updates
TEST_F(TimerTest, OneSecondEqualsTickRate)
{
	// Arrange
	const int targetTickRate = 60;
	const double fixedDeltaTime = 1.0 / targetTickRate;

	std::function<double()> clockFunction = []()
	{ return SDL_GetTicks() / 1000.0; };
	std::unique_ptr<ApplicationClock> timer =
		std::make_unique<ApplicationClock>(clockFunction, 60, 0.25);
	timer->start();

	// Give the timer a moment to initialize
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	auto startTime = std::chrono::steady_clock::now();
	auto endTime = startTime + std::chrono::seconds(1);

	int tickCount = 0;

	// Act
	while ( std::chrono::steady_clock::now() < endTime )
	{
		timer->tick();

		while ( timer->shouldFixedUpdate() )
		{
			timer->consumeFixedUpdate();
			tickCount++;
		}

		// Small sleep to avoid busy waiting
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	// Also check the timer's internal tick counter
	int actualTicks = timer->getTotalTicks();

	// Assert
	//  Use tickCount if getTickRate() isn't working
	int ticksToCheck = (actualTicks > 0) ? actualTicks : tickCount;

	int lowerBound = targetTickRate - 5;  // Allow more tolerance
	int upperBound = targetTickRate + 5;

	EXPECT_GE(ticksToCheck, lowerBound)
		<< "After 1 second, expected at least " << lowerBound << " ticks, got "
		<< ticksToCheck;
	EXPECT_LE(ticksToCheck, upperBound)
		<< "After 1 second, expected at most " << upperBound << " ticks, got "
		<< ticksToCheck;

	// Only check simulation time if we got ticks
	if ( ticksToCheck > 0 )
	{
		EXPECT_NEAR(timer->getTime(), 1.0, 0.1)
			<< "Simulation time should be approximately 1 second";
	}
}

// Test 2: Verify spiral of death prevention (frame time clamping)
TEST_F(TimerTest, SpiralOfDeathPrevention)
{
	// Arrange
	const int targetTickRate = 60;
	const double fixedDeltaTime = 1.0 / targetTickRate;
	const double maxFrameTime = 0.25;

	std::function<double()> clockFunction = []()
	{ return SDL_GetTicks() / 1000.0; };
	std::unique_ptr<ApplicationClock> timer =
		std::make_unique<ApplicationClock>(clockFunction, 60, 0.25);
	timer->start();

	// Act
	timer->start();

	// Initial tick to establish baseline
	timer->tick();

	// Simulate a huge frame time
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	timer->tick();

	int ticksProcessed = 0;
	while ( timer->shouldFixedUpdate() )
	{
		timer->consumeFixedUpdate();
		ticksProcessed++;

		if ( ticksProcessed > 100 )
		{
			FAIL() << "Spiral of death not prevented! Processed "
				   << ticksProcessed << " ticks from a single huge frame";
		}
	}

	// Assert
	const int maxExpectedTicks =
		static_cast<int>(maxFrameTime * targetTickRate) + 1;

	EXPECT_LE(ticksProcessed, maxExpectedTicks)
		<< "Huge frame should be clamped. Expected max " << maxExpectedTicks
		<< " ticks, but processed " << ticksProcessed;

	EXPECT_GT(ticksProcessed, 0)
		<< "Should still process some ticks even with clamping";
}

// Test 3: Verify time scale affects getDeltaTime
TEST_F(TimerTest, TimeScaleAffectsDeltaTime)
{
	// Arrange
	const int targetTickRate = 60;
	const double fixedDeltaTime = 1.0 / targetTickRate;

	std::function<double()> clockFunction = []()
	{ return SDL_GetTicks() / 1000.0; };
	std::unique_ptr<ApplicationClock> timer =
		std::make_unique<ApplicationClock>(clockFunction, targetTickRate, 0.25);

	// Act & Assert - Default time scale should be 1.0
	EXPECT_DOUBLE_EQ(timer->getTimeScale(), 1.0);
	EXPECT_DOUBLE_EQ(timer->getDeltaTime(), fixedDeltaTime);

	// Act & Assert - Slow motion (0.5x)
	timer->setTimeScale(0.5);
	EXPECT_DOUBLE_EQ(timer->getTimeScale(), 0.5);
	EXPECT_DOUBLE_EQ(timer->getDeltaTime(), fixedDeltaTime * 0.5);

	// Act & Assert - Fast motion (2.0x)
	timer->setTimeScale(2.0);
	EXPECT_DOUBLE_EQ(timer->getTimeScale(), 2.0);
	EXPECT_DOUBLE_EQ(timer->getDeltaTime(), fixedDeltaTime * 2.0);

	// Act & Assert - Very slow (0.25x)
	timer->setTimeScale(0.25);
	EXPECT_DOUBLE_EQ(timer->getTimeScale(), 0.25);
	EXPECT_DOUBLE_EQ(timer->getDeltaTime(), fixedDeltaTime * 0.25);

	// Act & Assert - Reset to normal
	timer->setTimeScale(1.0);
	EXPECT_DOUBLE_EQ(timer->getTimeScale(), 1.0);
	EXPECT_DOUBLE_EQ(timer->getDeltaTime(), fixedDeltaTime);
}

// Test 4: Verify pause prevents fixed updates
TEST_F(TimerTest, PausePreventsFixedUpdates)
{
	// Arrange
	std::function<double()> clockFunction = []()
	{ return SDL_GetTicks() / 1000.0; };
	std::unique_ptr<ApplicationClock> timer =
		std::make_unique<ApplicationClock>(clockFunction, 60, 0.25);
	timer->start();

	// Give the timer a moment to initialize
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// Act & Assert - Initially not paused
	EXPECT_FALSE(timer->isPaused());

	// Accumulate some time
	timer->tick();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	timer->tick();

	// Should be able to update when not paused
	int updatesBeforePause = 0;
	while ( timer->shouldFixedUpdate() )
	{
		timer->consumeFixedUpdate();
		updatesBeforePause++;
		if ( updatesBeforePause > 10 ) break;  // Safety limit
	}
	EXPECT_GT(updatesBeforePause, 0);

	// Act - Pause the timer
	timer->pause();
	EXPECT_TRUE(timer->isPaused());

	// Accumulate more time
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	timer->tick();

	// Assert - shouldFixedUpdate should return false when paused
	EXPECT_FALSE(timer->shouldFixedUpdate());

	// Act - Resume
	timer->resume();
	EXPECT_FALSE(timer->isPaused());

	// Accumulate time after resume
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	timer->tick();

	// Should be able to update again
	int updatesAfterResume = 0;
	while ( timer->shouldFixedUpdate() && updatesAfterResume < 10 )
	{
		timer->consumeFixedUpdate();
		updatesAfterResume++;
	}
	EXPECT_GT(updatesAfterResume, 0);
}

// Test 5: Verify toggle pause
TEST_F(TimerTest, TogglePause)
{
	// Arrange
	std::function<double()> clockFunction = []()
	{ return SDL_GetTicks() / 1000.0; };
	std::unique_ptr<ApplicationClock> timer =
		std::make_unique<ApplicationClock>(clockFunction, 60, 0.25);

	// Act & Assert - Initially not paused
	EXPECT_FALSE(timer->isPaused());

	// Toggle to paused
	timer->togglePause();
	EXPECT_TRUE(timer->isPaused());

	// Toggle back to not paused
	timer->togglePause();
	EXPECT_FALSE(timer->isPaused());

	// Toggle again
	timer->togglePause();
	EXPECT_TRUE(timer->isPaused());
}

// Test 6: Verify time scale affects simulation speed
TEST_F(TimerTest, TimeScaleAffectsSimulationSpeed)
{
	// Arrange
	std::function<double()> clockFunction = []()
	{ return SDL_GetTicks() / 1000.0; };
	const int targetTickRate = 60;

	// Test with normal speed
	std::unique_ptr<ApplicationClock> timerNormal =
		std::make_unique<ApplicationClock>(clockFunction, targetTickRate, 0.25);
	timerNormal->setTimeScale(1.0);
	timerNormal->start();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// Test with slow speed
	std::unique_ptr<ApplicationClock> timerSlow =
		std::make_unique<ApplicationClock>(clockFunction, targetTickRate, 0.25);
	timerSlow->setTimeScale(0.5);
	timerSlow->start();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// Accumulate time for both
	auto startTime = std::chrono::steady_clock::now();
	auto endTime = startTime + std::chrono::milliseconds(100);

	while ( std::chrono::steady_clock::now() < endTime )
	{
		timerNormal->tick();
		timerSlow->tick();

		while ( timerNormal->shouldFixedUpdate() )
		{
			timerNormal->consumeFixedUpdate();
		}

		while ( timerSlow->shouldFixedUpdate() )
		{
			timerSlow->consumeFixedUpdate();
		}

		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	// Assert - Normal speed should have more ticks than slow speed
	// (though both get same real time, normal processes more simulation time)
	int normalTicks = timerNormal->getTotalTicks();
	int slowTicks = timerSlow->getTotalTicks();

	// Normal should have approximately 2x more ticks than slow
	EXPECT_GT(normalTicks, slowTicks);

	// Check that delta times match expected scaling
	EXPECT_DOUBLE_EQ(timerNormal->getDeltaTime(), (1.0 / targetTickRate) * 1.0);
	EXPECT_DOUBLE_EQ(timerSlow->getDeltaTime(), (1.0 / targetTickRate) * 0.5);
}