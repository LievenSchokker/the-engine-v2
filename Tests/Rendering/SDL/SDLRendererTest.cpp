#include "External/SDLBackendContext.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"

#include <gtest/gtest.h>

class SDLRendererTest: public ::testing::Test
{
   protected:
	SDLBackendContext* ctx;

	void SetUp() override
	{
		ctx = new SDLBackendContext(SDL_INIT_VIDEO);
	}

	void TearDown() override
	{
		delete ctx;
	}
};

// Test 1: Test open and close functionality
TEST_F(SDLRendererTest, OpenCloseTest)
{
	SDLRenderer render(*ctx);
	EXPECT_FALSE(render.isOpen()) << "Window should not be open initially";

	WindowOptions opts{"Test Window", 640, 480};

	render.open(opts);

	EXPECT_TRUE(render.isOpen()) << "Window should open after calling open()";

	render.close();
	EXPECT_FALSE(render.isOpen())
		<< "Window should be closed after calling close()";
}