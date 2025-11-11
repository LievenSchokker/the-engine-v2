#include <gtest/gtest.h>
#include "core/rendering/SDL/SDLRender.h"
#include "external/SdlContext.h"


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

    // Initially, window should not be open
    EXPECT_FALSE(render.isOpen()) << "Window should not be open initially";

    WindowOptions opts{"Test Window", 640, 480};
    render.open(opts);

    EXPECT_TRUE(render.isOpen()) << "Window should be open after calling open()";

    // Close the window
    render.close();

    // Window should now be closed
    EXPECT_FALSE(render.isOpen()) << "Window should be closed after calling close()";
}