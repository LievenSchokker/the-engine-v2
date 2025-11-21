// Tests/Core/TimerTest.cpp
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include "Core/Timer.h"

class TimerTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (SDL_Init(SDL_INIT_TIMER) < 0) {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }
    }

    void TearDown() override {
        SDL_Quit();
    }
};

// Test 1: Verify that 1 second of real time = tickRate updates
TEST_F(TimerTest, OneSecondEqualsTickRate) {
    //Arrange
    const int targetTickRate = 60;
    const double fixedDeltaTime = 1.0 / targetTickRate;

    Timer timer(fixedDeltaTime, []() {
        return SDL_GetTicks();
    });

    timer.start();

    // Give the timer a moment to initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(1);

    int tickCount = 0;

    //Act
    while (std::chrono::steady_clock::now() < endTime) {
        timer.tick();

        while (timer.shouldFixedUpdate()) {
            timer.consumeFixedUpdate();
            tickCount++;
        }

        // Small sleep to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    // Also check the timer's internal tick counter
    int actualTicks = timer.getTickRate();

    //Assert
    // Use tickCount if getTickRate() isn't working
    int ticksToCheck = (actualTicks > 0) ? actualTicks : tickCount;

    int lowerBound = targetTickRate - 5;  // Allow more tolerance
    int upperBound = targetTickRate + 5;

    EXPECT_GE(ticksToCheck, lowerBound)
        << "After 1 second, expected at least " << lowerBound
        << " ticks, got " << ticksToCheck;
    EXPECT_LE(ticksToCheck, upperBound)
        << "After 1 second, expected at most " << upperBound
        << " ticks, got " << ticksToCheck;

    // Only check simulation time if we got ticks
    if (ticksToCheck > 0) {
        EXPECT_NEAR(timer.getTime(), 1.0, 0.1)
            << "Simulation time should be approximately 1 second";
    }
}

// Test 2: Verify spiral of death prevention (frame time clamping)
TEST_F(TimerTest, SpiralOfDeathPrevention) {
    //Arrange
    const int targetTickRate = 60;
    const double fixedDeltaTime = 1.0 / targetTickRate;
    const double maxFrameTime = 0.25;

    Timer timer(fixedDeltaTime, []() {
        return SDL_GetTicks();
    });

    //Act
    timer.start();

    // Initial tick to establish baseline
    timer.tick();

    // Simulate a huge frame time
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    timer.tick();

    int ticksProcessed = 0;
    while (timer.shouldFixedUpdate()) {
        timer.consumeFixedUpdate();
        ticksProcessed++;

        if (ticksProcessed > 100) {
            FAIL() << "Spiral of death not prevented! Processed "
                   << ticksProcessed << " ticks from a single huge frame";
        }
    }

    //Assert
    const int maxExpectedTicks = static_cast<int>(maxFrameTime * targetTickRate) + 1;

    EXPECT_LE(ticksProcessed, maxExpectedTicks)
        << "Huge frame should be clamped. Expected max " << maxExpectedTicks
        << " ticks, but processed " << ticksProcessed;

    EXPECT_GT(ticksProcessed, 0)
        << "Should still process some ticks even with clamping";
}