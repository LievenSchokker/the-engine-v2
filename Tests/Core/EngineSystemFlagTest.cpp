#include <gtest/gtest.h>
#include "Core/Options/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"
#include "Game.h"
#include "Scene/Scene.h"

class EngineSystemCombinationsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        specs = ApplicationSpecifications{};
        specs.renderBackend = RenderBackend::SDL;
        specs.windowOptions = {"Test", 100, 100};
        specs.networkingOptions.port = 7777;
        specs.networkingOptions.serverIP = "127.0.0.1";
        specs.networkingOptions.tickRate = 60;
        specs.maxFrameTime = 0.25;
    }

    void testCombination(EngineSystem system)
    {
        specs.engineSystem = system;

        auto game = std::make_unique<Game>();
        game->setApplicationSpecifications(specs);
        game->addScene(std::make_unique<Scene>("TestScene"));

        SpelMotor engine(std::move(game));
        engine.initialize();

        constexpr int frameCount = 5;
        for (int i = 0; i < frameCount; ++i)
        {
            engine.tick();
        }

        engine.shutdown();
    }

    ApplicationSpecifications specs;
};

TEST_F(EngineSystemCombinationsTest, Renderer_Only)
{
    testCombination(EngineSystem::Renderer);
}

TEST_F(EngineSystemCombinationsTest, Physics_Only)
{
    testCombination(EngineSystem::Physics);
}

TEST_F(EngineSystemCombinationsTest, Audio_Only)
{
    testCombination(EngineSystem::Audio);
}

TEST_F(EngineSystemCombinationsTest, Input_Only)
{
    testCombination(EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, NetClient_Only)
{
    testCombination(EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, NetServer_Only)
{
    testCombination(EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Audio)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Audio);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Input)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, Renderer_NetClient)
{
    testCombination(EngineSystem::Renderer | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Renderer_NetServer)
{
    testCombination(EngineSystem::Renderer | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Physics_Audio)
{
    testCombination(EngineSystem::Physics | EngineSystem::Audio);
}

TEST_F(EngineSystemCombinationsTest, Physics_Input)
{
    testCombination(EngineSystem::Physics | EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, Physics_NetClient)
{
    testCombination(EngineSystem::Physics | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Physics_NetServer)
{
    testCombination(EngineSystem::Physics | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Audio_Input)
{
    testCombination(EngineSystem::Audio | EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, Audio_NetClient)
{
    testCombination(EngineSystem::Audio | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Audio_NetServer)
{
    testCombination(EngineSystem::Audio | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Input_NetClient)
{
    testCombination(EngineSystem::Input | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Input_NetServer)
{
    testCombination(EngineSystem::Input | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, NetClient_NetServer)
{
    testCombination(EngineSystem::NetClient | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_Audio)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::Audio);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_Input)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_NetClient)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_NetServer)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Audio_Input)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Audio | EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Audio_NetClient)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Audio | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Physics_Audio_Input)
{
    testCombination(EngineSystem::Physics | EngineSystem::Audio | EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, Physics_Audio_NetClient)
{
    testCombination(EngineSystem::Physics | EngineSystem::Audio | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Physics_Audio_NetServer)
{
    testCombination(EngineSystem::Physics | EngineSystem::Audio | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_Audio_Input)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::Audio | EngineSystem::Input);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_Audio_NetClient)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::Audio | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_Audio_NetServer)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::Audio | EngineSystem::NetServer);
}

TEST_F(EngineSystemCombinationsTest, Renderer_Physics_Input_NetClient)
{
    testCombination(EngineSystem::Renderer | EngineSystem::Physics | EngineSystem::Input | EngineSystem::NetClient);
}

TEST_F(EngineSystemCombinationsTest, Client_Preset)
{
    testCombination(EngineSystem::Client);
}

TEST_F(EngineSystemCombinationsTest, Server_Preset)
{
    testCombination(EngineSystem::Server);
}

TEST_F(EngineSystemCombinationsTest, All_Flags)
{
    testCombination(
        EngineSystem::Renderer | EngineSystem::Physics |
        EngineSystem::Audio | EngineSystem::Input |
        EngineSystem::NetClient
    );
}

TEST_F(EngineSystemCombinationsTest, All_Flags_With_Both_Network)
{
    testCombination(
        EngineSystem::Renderer | EngineSystem::Physics |
        EngineSystem::Audio | EngineSystem::Input |
        EngineSystem::NetClient | EngineSystem::NetServer
    );
}