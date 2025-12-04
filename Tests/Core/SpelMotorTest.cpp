// Tests/Core/SpelMotorTest.cpp
#include "Game.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <cstdlib>  // for setenv/unsetenv
#include <SDL2/SDL.h>
#include "Core/SpelMotor.h"
#include "Core/ApplicationSpecifications.h"

class SpelMotorTest : public ::testing::Test
{
public:
    ApplicationSpecifications specifications{};
	std::unique_ptr<Game> game = std::make_unique<Game>();
protected:
    void SetUp() override
    {
        if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0)
        {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }
        specifications.windowOptions = {"Test Window", 800, 600};
        specifications.renderBackend = RenderBackend::SDL;
    	game->setApplicationSpecifications(specifications);
    }

    void TearDown() override
    {
        SDL_Quit();
    }

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
    auto* engine = new SpelMotor(std::make_unique<Game>());

    std::thread quitThread([]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        SDL_Event quitEvent;
        quitEvent.type = SDL_QUIT;
        SDL_PushEvent(&quitEvent);
    });
    quitThread.detach();

    //Assert
    ASSERT_NO_THROW({
        engine->shutdown();
    });

    delete engine;
    SUCCEED();
}
