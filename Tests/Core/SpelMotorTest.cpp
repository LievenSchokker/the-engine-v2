// Tests/Core/SpelMotorTest.cpp
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include "Core/SpelMotor.h"
#include "Core/ApplicationSpecifications.h"

class SpelMotorTest : public ::testing::Test
{
protected:
    ApplicationSpecifications specifications{};

    void SetUp() override
    {
        if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0)
        {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }
        specifications.windowOptions = {"Test Window", 800, 600};
        specifications.renderBackend = RenderBackend::SDL;
        specifications.networkingOptions.tickRate = 60;
    	specifications.networkingOptions.port = 7777;
    	specifications.networkingOptions.serverIP = "127.0.0.1";
        specifications.maxFrameTime = 0.25;
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


TEST_F(SpelMotorTest, ConstructionInitializesEngineLoop)
{
    ASSERT_NO_THROW({
        SpelMotor engine(specifications);
    });
}

TEST_F(SpelMotorTest, ThrowsOnInvalidConfiguration)
{
    ApplicationSpecifications invalidSpecs{};
    invalidSpecs.renderBackend = static_cast<RenderBackend>(-1);  // Invalid

    ASSERT_THROW({
        SpelMotor engine(invalidSpecs);
    }, std::runtime_error);
}