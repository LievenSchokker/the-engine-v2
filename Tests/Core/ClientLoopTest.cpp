#include "Core/EngineLoops/ClientLoop.h"
#include "Core/EngineLoops/ServerLoop.h"
#include "Game.h"

#include <SDL_timer.h>
#include <gtest/gtest.h>
#include <memory>

class ClientLoopTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		ApplicationSpecifications specs;
		specs.networkingOptions.tickRate = 60;
		specs.renderBackend = RenderBackend::SDL;
		specs.windowOptions.width = 800;
		specs.windowOptions.height = 600;
		specs.windowOptions.title = "Test Window";
		std::unique_ptr<Game> game = std::make_unique<Game>();

		auto testScene = std::make_unique<Scene>("TestScene");
		game->addScene(std::move(testScene));
		game->setApplicationSpecifications(specs);
		clientLoop = std::make_unique<ClientLoop>(std::move(game));
	}

	void TearDown() override
	{
		if (clientLoop)
		{
			clientLoop->shutdown();
		}
	}

	std::unique_ptr<ClientLoop> clientLoop;
};

TEST_F(ClientLoopTest, ConstructionInitializesSubsystems)
{
	// Verify all core subsystems are created and accessible
	EXPECT_NE(clientLoop->getGameWorld(), nullptr);
	EXPECT_NE(clientLoop->getSceneManager(), nullptr);
}

TEST_F(ClientLoopTest, ClockReturnsIncreasingTime)
{
	auto clock = clientLoop->getClock();

	double firstSample = clock();

	// Small delay to ensure measurable time difference
	SDL_Delay(10);

	double secondSample = clock();

	EXPECT_GT(secondSample, firstSample);
}