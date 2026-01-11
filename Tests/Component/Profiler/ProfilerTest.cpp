#include "Component/Profiler/Profiler.h"
#include "Core/GameWorld.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

#include <cmath>
#include <gtest/gtest.h>
#include <vector>

class MockUIRenderQueueWriter : public IUserInterfaceRenderQueueWriter
{
public:
    std::vector<UIRenderCommand> commands;

    void push(const UIRenderCommand command) override
    {
        commands.push_back(command);
    }

    void clear()
    {
        commands.clear();
    }
};

class ProfilerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        profiler = std::make_unique<Profiler>(10.0f, 20.0f, 300.0f, 400.0f);
        mockQueue = std::make_unique<MockUIRenderQueueWriter>();
    }

    void TearDown() override
    {
        profiler.reset();
        mockQueue.reset();
    }

    void simulateFrames(int count, float deltaTime)
    {
        auto world = std::make_unique<GameWorld>();
        for (int i = 0; i < count; ++i)
        {
            profiler->update(deltaTime, *world);
        }
    }

    bool hasTextContaining(const std::string& substring) const
    {
        for (const auto& cmd : mockQueue->commands)
        {
            if (cmd.type == UICommandType::Text &&
                cmd.text.find(substring) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }

    const UIRenderCommand* findTextContaining(const std::string& substring) const
    {
        for (const auto& cmd : mockQueue->commands)
        {
            if (cmd.type == UICommandType::Text &&
                cmd.text.find(substring) != std::string::npos)
            {
                return &cmd;
            }
        }
        return nullptr;
    }

    std::unique_ptr<Profiler> profiler;
    std::unique_ptr<MockUIRenderQueueWriter> mockQueue;
};

TEST_F(ProfilerTest, ConstructorInitializesPosition)
{
    Profiler p(100.0f, 200.0f, 300.0f, 400.0f);

    UIRenderCommand panel = p.renderPanel();

    EXPECT_FLOAT_EQ(panel.x, 100.0f);
    EXPECT_FLOAT_EQ(panel.y, 200.0f);
    EXPECT_FLOAT_EQ(panel.width, 300.0f);
    EXPECT_FLOAT_EQ(panel.height, 400.0f);
}

TEST_F(ProfilerTest, RenderPanelHasCorrectType)
{
    UIRenderCommand panel = profiler->renderPanel();

    EXPECT_EQ(panel.type, UICommandType::Panel);
}

TEST_F(ProfilerTest, RenderPanelHasCorrectTitle)
{
    UIRenderCommand panel = profiler->renderPanel();

    EXPECT_EQ(panel.title, "Debug Profiler");
    EXPECT_TRUE(panel.hasTitle);
    EXPECT_TRUE(panel.hasBorder);
}

TEST_F(ProfilerTest, RenderPanelHasCorrectPanelId)
{
    UIRenderCommand panel = profiler->renderPanel();

    EXPECT_EQ(panel.panelId, 9999);
    EXPECT_EQ(panel.parentId, NO_PARENT);
}

TEST_F(ProfilerTest, RenderPanelHasLayoutSettings)
{
    UIRenderCommand panel = profiler->renderPanel();

    EXPECT_FLOAT_EQ(panel.rowHeight, 20.0f);
    EXPECT_EQ(panel.columns, 1);
}


TEST_F(ProfilerTest, ToggleChangesVisibility)
{
    profiler->fillUserInterfaceRenderQueue(*mockQueue);
    bool initiallyHadCommands = !mockQueue->commands.empty();

    mockQueue->clear();
    profiler->toggle();
    profiler->fillUserInterfaceRenderQueue(*mockQueue);
    bool hasCommandsAfterToggle = !mockQueue->commands.empty();

    EXPECT_NE(initiallyHadCommands, hasCommandsAfterToggle);
}

TEST_F(ProfilerTest, ToggleTwiceRestoresVisibility)
{
    profiler->fillUserInterfaceRenderQueue(*mockQueue);
    size_t initialCount = mockQueue->commands.size();

    profiler->toggle();
    profiler->toggle();

    mockQueue->clear();
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    EXPECT_EQ(mockQueue->commands.size(), initialCount);
}

TEST_F(ProfilerTest, RenderFpsReturnsTextCommand)
{
    UIRenderCommand fps = profiler->renderFps();

    EXPECT_EQ(fps.type, UICommandType::Text);
    EXPECT_EQ(fps.panelId, 9999);
    EXPECT_EQ(fps.alignment, Alignment::Left);
}

TEST_F(ProfilerTest, RenderFpsContainsFPSLabel)
{
    UIRenderCommand fps = profiler->renderFps();

    EXPECT_NE(fps.text.find("FPS:"), std::string::npos);
}

TEST_F(ProfilerTest, RenderFpsShowsGreenColorForHighFPS)
{
    simulateFrames(100, 1.0f / 120.0f);

    UIRenderCommand fps = profiler->renderFps();

    EXPECT_EQ(fps.color, Color::green());
}

TEST_F(ProfilerTest, RenderFpsShowsYellowColorForMidFPS)
{
    simulateFrames(100, 1.0f / 45.0f);

    UIRenderCommand fps = profiler->renderFps();

    EXPECT_EQ(fps.color, Color::yellow());
}

TEST_F(ProfilerTest, RenderFpsShowsRedColorForLowFPS)
{
    simulateFrames(100, 1.0f / 15.0f);

    UIRenderCommand fps = profiler->renderFps();

    EXPECT_EQ(fps.color, Color::red());
}

TEST_F(ProfilerTest, RenderFrameTimeReturnsTextCommand)
{
    UIRenderCommand frameTime = profiler->renderFrameTime();

    EXPECT_EQ(frameTime.type, UICommandType::Text);
    EXPECT_EQ(frameTime.panelId, 9999);
}

TEST_F(ProfilerTest, RenderFrameTimeContainsFrameLabel)
{
    UIRenderCommand frameTime = profiler->renderFrameTime();

    EXPECT_NE(frameTime.text.find("Frame:"), std::string::npos);
    EXPECT_NE(frameTime.text.find("ms"), std::string::npos);
}

TEST_F(ProfilerTest, RenderFrameTimeShowsGreenForFastFrames)
{
    simulateFrames(100, 0.010f);

    UIRenderCommand frameTime = profiler->renderFrameTime();

    EXPECT_EQ(frameTime.color, Color::green());
}

TEST_F(ProfilerTest, RenderFrameTimeShowsYellowForMediumFrames)
{
    // 25ms frames (between 16ms and 33ms)
    simulateFrames(100, 0.025f);

    UIRenderCommand frameTime = profiler->renderFrameTime();

    EXPECT_EQ(frameTime.color, Color::yellow());
}

TEST_F(ProfilerTest, RenderFrameTimeShowsRedForSlowFrames)
{
    simulateFrames(100, 0.050f);

    UIRenderCommand frameTime = profiler->renderFrameTime();

    EXPECT_EQ(frameTime.color, Color::red());
}



TEST_F(ProfilerTest, RenderMinMaxLabelReturnsTextCommand)
{
    UIRenderCommand minMax = profiler->renderMinMaxLabel();

    EXPECT_EQ(minMax.type, UICommandType::Text);
    EXPECT_EQ(minMax.panelId, 9999);
}

TEST_F(ProfilerTest, RenderMinMaxLabelContainsMinMax)
{
    UIRenderCommand minMax = profiler->renderMinMaxLabel();

    EXPECT_NE(minMax.text.find("Min/Max:"), std::string::npos);
    EXPECT_NE(minMax.text.find("ms"), std::string::npos);
}

TEST_F(ProfilerTest, RenderMinMaxLabelTracksVariation)
{
    auto world = std::make_unique<GameWorld>();
    profiler->update(0.010f, *world);
    profiler->update(0.030f, *world);
    profiler->update(0.020f, *world);

    UIRenderCommand minMax = profiler->renderMinMaxLabel();

    EXPECT_NE(minMax.text.find("10.00"), std::string::npos);
    EXPECT_NE(minMax.text.find("30.00"), std::string::npos);
}

TEST_F(ProfilerTest, RenderMinMaxLabelUsesLabelColor)
{
    UIRenderCommand minMax = profiler->renderMinMaxLabel();

    EXPECT_EQ(minMax.color, Color::white());
}


TEST_F(ProfilerTest, SetShowFPSFalseHidesFPS)
{
    profiler->setShowFPS(false);
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    EXPECT_FALSE(hasTextContaining("FPS:"));
}

TEST_F(ProfilerTest, SetShowFPSTrueShowsFPS)
{
    profiler->setShowFPS(true);
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    EXPECT_TRUE(hasTextContaining("FPS:"));
}

TEST_F(ProfilerTest, SetShowFrameTimeTrueShowsFrameTime)
{
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    EXPECT_TRUE(hasTextContaining("Frame:"));
    EXPECT_TRUE(hasTextContaining("Min/Max:"));
}

TEST_F(ProfilerTest, SetShowEntityCountFalseHidesEntityInfo)
{
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    EXPECT_FALSE(hasTextContaining("Entities:"));
    EXPECT_FALSE(hasTextContaining("Active Scenes:"));
}

TEST_F(ProfilerTest, RenderQueueNotFilledWhenInvisible)
{
    profiler->toggle();
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    EXPECT_TRUE(mockQueue->commands.empty());
}

TEST_F(ProfilerTest, RenderQueueStartsWithPanel)
{
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    ASSERT_FALSE(mockQueue->commands.empty());
    EXPECT_EQ(mockQueue->commands[0].type, UICommandType::Panel);
    EXPECT_EQ(mockQueue->commands[0].title, "Debug Profiler");
}

TEST_F(ProfilerTest, RenderQueueContainsSeparator)
{
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    bool foundSeparator = false;
    for (const auto& cmd : mockQueue->commands)
    {
        if (cmd.type == UICommandType::Separator)
        {
            foundSeparator = true;
            break;
        }
    }

    EXPECT_TRUE(foundSeparator);
}

TEST_F(ProfilerTest, RenderQueueAllCommandsUseSamePanelId)
{
    profiler->fillUserInterfaceRenderQueue(*mockQueue);

    for (const auto& cmd : mockQueue->commands)
    {
        EXPECT_EQ(cmd.panelId, 9999);
    }
}

TEST_F(ProfilerTest, FPSCalculationWithConstantFrameTime)
{
    // Simulate 60 FPS
    simulateFrames(100, 1.0f / 60.0f);

    UIRenderCommand fps = profiler->renderFps();

    // Text should contain a value near 60
    EXPECT_NE(fps.text.find("60"), std::string::npos);
}

TEST_F(ProfilerTest, FPSCalculationWithVariableFrameTime)
{
    // Mix of 60 FPS and 30 FPS frames
    simulateFrames(30, 1.0f / 60.0f);
    simulateFrames(30, 1.0f / 30.0f);

    UIRenderCommand fps = profiler->renderFps();

    // Should show yellow (between 30 and 60 FPS)
    EXPECT_EQ(fps.color, Color::yellow());
}

TEST_F(ProfilerTest, HandleZeroDeltaTime)
{
    auto world = std::make_unique<GameWorld>();
    EXPECT_NO_THROW(profiler->update(0.0f, *world));

    UIRenderCommand fps = profiler->renderFps();
    EXPECT_EQ(fps.type, UICommandType::Text);
}

TEST_F(ProfilerTest, HandleVerySmallDeltaTime)
{
    auto world = std::make_unique<GameWorld>();
    EXPECT_NO_THROW(profiler->update(0.0001f, *world));

    UIRenderCommand fps = profiler->renderFps();
    EXPECT_EQ(fps.type, UICommandType::Text);
}

TEST_F(ProfilerTest, HandleVeryLargeDeltaTime)
{
    auto world = std::make_unique<GameWorld>();
    EXPECT_NO_THROW(profiler->update(10.0f, *world));

    UIRenderCommand fps = profiler->renderFps();
    // Should show red for very slow frames
    EXPECT_EQ(fps.color, Color::red());
}

TEST_F(ProfilerTest, HandleNullGameWorld)
{
    auto world = std::make_unique<GameWorld>();
    EXPECT_NO_THROW(profiler->update(0.016f, *world));
}

TEST_F(ProfilerTest, ManyFramesDoNotCrash)
{
    EXPECT_NO_THROW(simulateFrames(1000, 0.016f));
}

TEST_F(ProfilerTest, RapidToggleDoesNotCrash)
{
    for (int i = 0; i < 100; ++i)
    {
        profiler->toggle();
    }
    SUCCEED();
}

TEST_F(ProfilerTest, SetUpdateIntervalDoesNotCrash)
{
    EXPECT_NO_THROW(profiler->setUpdateInterval(0.5f));
    EXPECT_NO_THROW(profiler->setUpdateInterval(0.0f));
    EXPECT_NO_THROW(profiler->setUpdateInterval(10.0f));
}

TEST_F(ProfilerTest, ProfilerCanBeUsedAsBehaviour)
{
    auto world = std::make_unique<GameWorld>();
    Behaviour* behaviour = profiler.get();

    EXPECT_NO_THROW(behaviour->update(0.016f, *world));
}

TEST_F(ProfilerTest, ProfilerCanBeUsedAsUIObject)
{
    UIElement* uiObject = profiler.get();

    MockUIRenderQueueWriter queue;
    EXPECT_NO_THROW(uiObject->fillUserInterfaceRenderQueue(queue));
}