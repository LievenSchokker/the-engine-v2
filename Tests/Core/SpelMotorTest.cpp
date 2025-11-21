// Tests/Core/SpelMotorTest.cpp
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <cstdlib>  // for setenv/unsetenv
#include <SDL2/SDL.h>
#include "Core/SpelMotor.h"
#include "Core/ApplicationSpecifications.h"

class SpelMotorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Set environment variable - this takes precedence over everything
#ifdef _WIN32
        _putenv_s("SDL_VIDEODRIVER", "dummy");
#else
        setenv("SDL_VIDEODRIVER", "dummy", 1);
#endif

        if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0)
        {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }

        specifications.windowOptions = {"Test Window", 800, 600};
        specifications.renderBackend = RenderBackend::SDL;
    }

    void TearDown() override
    {
#ifdef _WIN32
        _putenv_s("SDL_VIDEODRIVER", "");
#else
        unsetenv("SDL_VIDEODRIVER");
#endif
        SDL_Quit();
    }

    // Member variables
    ApplicationSpecifications specifications {};

    // Helper function to push quit event after delay
    static void pushQuitEventAfterDelay(int milliseconds)
    {
        std::thread([milliseconds]()
        {
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
TEST_F(SpelMotorTest, CleanSubsystemShutdownViaQuitEvent)
{
    //Arrange
    auto* engine = new SpelMotor(specifications);

#ifdef __APPLE__
    // On macOS, even with dummy driver, it's safer to run on main thread
    // Push quit event from another thread
    std::thread quitThread([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        SDL_Event quitEvent;
        quitEvent.type = SDL_QUIT;
        SDL_PushEvent(&quitEvent);
    });
    quitThread.detach();

    //Act - Run on main thread
    engine->run();
#else
    // Other platforms can safely use threads
    pushQuitEventAfterDelay(100);

    //Act
    std::thread engineThread([engine]()
    {
        engine->run();
    });

    if (engineThread.joinable())
    {
        engineThread.join();
    }
#endif

    //Assert
    ASSERT_NO_THROW({
        engine->shutdown();
    });

    delete engine;
    SUCCEED();
}