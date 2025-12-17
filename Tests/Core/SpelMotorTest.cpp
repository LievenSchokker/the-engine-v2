// Tests/Core/SpelMotorTest.cpp
#include "Game.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include "Core/SpelMotor.h"
#include "../../Engine/inc/Core/Options/ApplicationSpecifications.h"

class SpelMotorTest : public ::testing::Test
{
protected:
    ApplicationSpecifications specifications{};
	std::unique_ptr<Game> game;
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
    	game = std::make_unique<Game>();
    	std::unique_ptr<Scene> scene = std::make_unique<Scene>("Yoo");
    	game->addScene(std::move(scene));
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

// Tests/Mocks/MockEngineLoop.h
#pragma once

#include "Core/IEngineLoop.h"

class MockEngineLoop : public IEngineLoop
{
public:
    using ClockFunction = std::function<double()>;

    ClockFunction getClock() override
    {
        return []() { return 0.0; };
    }

    void start() override {}
    void update(double) override {}
    void fixedUpdate(double) override {}
    void shutdown() override {}
    void setApplicationClock(ApplicationClock*) override {}

    GameWorld* getGameWorld() override { return nullptr; }
    SceneManager* getSceneManager() override { return nullptr; }
};

TEST_F(SpelMotorTest, ConstructionInitializesEngineLoop)
{
    auto mockLoop = std::make_unique<MockEngineLoop>();

    ASSERT_NO_THROW({
        SpelMotor engine(std::move(game), std::move(mockLoop));
    });
}

TEST_F(SpelMotorTest, ThrowsOnInvalidConfiguration)
{
    ApplicationSpecifications invalidSpecs{};
    invalidSpecs.renderBackend = static_cast<RenderBackend>(-1);

    auto invalidGame = std::make_unique<Game>();
    invalidGame->setApplicationSpecifications(invalidSpecs);

    ASSERT_THROW({
        SpelMotor engine(std::move(invalidGame), nullptr);
    }, std::runtime_error);
}