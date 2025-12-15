#include <gtest/gtest.h>
#include "Core/EngineLoops/ServerLoop.h"
#include "Core/EngineLoops/ClientLoop.h"
#include "Core/ApplicationSpecifications.h"
#include "Game.h"

class ServerLoopTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		ApplicationSpecifications specs;
		specs.networkingOptions.tickRate = 60;
		specs.networkingOptions.port = 7777;

		std::unique_ptr<Game> game = std::make_unique<Game>();
		game->setApplicationSpecifications(specs);
		serverLoop = std::make_unique<ServerLoop>(std::move(game));
	}

	void TearDown() override
	{
		if (serverLoop)
		{
			serverLoop->shutdown();
		}
	}

	std::unique_ptr<ServerLoop> serverLoop;
};

TEST_F(ServerLoopTest, ConstructionInitializesSubsystems)
{
	// Verify all core subsystems are created and accessible
	EXPECT_NE(serverLoop->getGameWorld(), nullptr);
	EXPECT_NE(serverLoop->getSceneManager(), nullptr);
}

TEST_F(ServerLoopTest, ClockReturnsIncreasingTime)
{
	auto clock = serverLoop->getClock();

	double firstSample = clock();

	// Burn some CPU time to ensure measurable difference
	volatile int sum = 0;
	for (int i = 0; i < 100000; ++i)
	{
		sum += i;
	}

	double secondSample = clock();

	EXPECT_GT(secondSample, firstSample);
}