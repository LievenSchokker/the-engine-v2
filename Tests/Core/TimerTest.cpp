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

    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(1);

    //Act
    while (std::chrono::steady_clock::now() < endTime) {
        timer.tick();

        while (timer.shouldFixedUpdate()) {
            timer.consumeFixedUpdate();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    int actualTicks = timer.getTickRate();

    //Tick rate is faster then actual measurement this gives us a bit of extra margin.
    int lowerBound = targetTickRate - 2;
    int upperBound = targetTickRate + 2;

    //Assert
    EXPECT_GE(actualTicks, lowerBound)
        << "After 1 second, expected at least " << lowerBound << " ticks, got " << actualTicks;
    EXPECT_LE(actualTicks, upperBound)
        << "After 1 second, expected at most " << upperBound << " ticks, got " << actualTicks;

    EXPECT_NEAR(timer.getTime(), 1.0, 0.05)
        << "Simulation time should be approximately 1 second";
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
    const int maxExpectedTicks = static_cast<int>(maxFrameTime * targetTickRate);

    EXPECT_LE(ticksProcessed, maxExpectedTicks)
        << "Huge frame should be clamped. Expected max " << maxExpectedTicks
        << " ticks, but processed " << ticksProcessed;

    EXPECT_GT(ticksProcessed, 0)
        << "Should still process some ticks even with clamping";
}