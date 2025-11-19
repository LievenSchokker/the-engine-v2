#include <SDL.h>
#include <gtest/gtest.h>


#include "Rendering/SDL/SDLRender.h"
#include "External/SdlContext.h"
#include "Rendering/Window/WindowOptions.h"

class SDLRenderTest : public ::testing::Test {
protected:
    SdlContext* ctx;

    void SetUp() override
    {
        ctx = new SdlContext(SDL_INIT_VIDEO);
    }

    void TearDown() override
    {
        delete ctx;
    }
};

// Test 1: Test open and close functionality
TEST_F(SDLRenderTest, OpenCloseTest)
{
    SDLRender render(*ctx);
    EXPECT_FALSE(render.isOpen()) << "Window should not be open initially";

    WindowOptions opts{"Test Window", 640, 480};

    render.open(opts);

    EXPECT_TRUE(render.isOpen()) << "Window should open after calling open()";

    render.close();
    EXPECT_FALSE(render.isOpen()) << "Window should be closed after calling close()";
}