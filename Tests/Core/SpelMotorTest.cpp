// Tests/Core/SpelMotorTest.cpp
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include "Core/SpelMotor.h"
#include "Core/ApplicationSpecifications.h"

class SpelMotorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize SDL for testing (minimal subsystems)
        if (SDL_Init(SDL_INIT_EVENTS) < 0) {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }

        specifications.windowOptions = {"Test Window", 800, 600};
        specifications.renderBackend = RenderBackend::SDL;
    }

    void TearDown() override {
        SDL_Quit();
    }

    ApplicationSpecifications specifications {};

    //Can only ShutDown for now via SDL event will change later via our own event sytem.
    static void pushQuitEventAfterDelay(int milliseconds) {
        std::thread([milliseconds]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }).detach();
    }
};

//Ideally I would like to test if each system that has been called to init in run
// Also get's shutdown on Shutdown but I don't know how to mock that
// So this was the next best thing a came up with.

// Test 1: Cleanly destroying subsystems via SDL_QUIT event
TEST_F(SpelMotorTest, CleanSubsystemShutdownViaQuitEvent) {
    //Arrange
    auto* engine = new SpelMotor(specifications);
    pushQuitEventAfterDelay(100);

    //Act
    std::thread engineThread([engine]() {
        engine->run();
    });

    if (engineThread.joinable()) {
        engineThread.join();
    }

    //Assert
    ASSERT_NO_THROW({
        engine->shutdown();
    });

    SUCCEED();
}



