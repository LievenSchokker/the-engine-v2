#include "Rendering/Nuklear/NuklearSDLRenderHook.h"

#include <iostream>
#include <ostream>

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

//This is a nuklear specefic thing.
//Need to expose some sort of thing that Nuklear can use to store its context.
namespace
{
struct nk_context ctx;
}


NuklearSDLRenderHook::NuklearSDLRenderHook(SDL_Window* window,
                                           SDL_Renderer* renderer)
	: inputManager(InputManager::getInstance()), sdlWindow(window),
	  sdlRenderer(renderer), nuklearkContext(nullptr)
{
}

void NuklearSDLRenderHook::initialize()
{
	nuklearkContext = nk_sdl_init(sdlWindow, sdlRenderer);
	if (nuklearkContext != nullptr) {
		struct nk_font_atlas* atlas;
		nk_sdl_font_stash_begin(&atlas);
		nk_sdl_font_stash_end();

		nuklearkContext->style.window.fixed_background = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearkContext->style.window.fixed_background = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearkContext->style.window.header.normal = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearkContext->style.window.header.hover = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearkContext->style.window.header.active = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
	}
}

//Let me be  clear I DO NOT LIKE THIS.
//TODO Propper EventHandling so this can be fixed.
void NuklearSDLRenderHook::updateInput()
{
	if (!nuklearkContext) return;
	if (!inputManager) return;

	nk_input_begin(nuklearkContext);

	const int mx = inputManager->mouseX();
	const int my = inputManager->mouseY();
	nk_input_motion(nuklearkContext, mx, my);

	if (inputManager->wasMousePressed(MouseButton::LEFT)) nk_input_button(
		nuklearkContext, NK_BUTTON_LEFT, mx, my, 1);
	if (inputManager->wasMouseReleased(MouseButton::LEFT)) nk_input_button(
		nuklearkContext, NK_BUTTON_LEFT, mx, my, 0);

	if (inputManager->wasMousePressed(MouseButton::MIDDLE)) nk_input_button(
		nuklearkContext, NK_BUTTON_MIDDLE, mx, my, 1);
	if (inputManager->wasMouseReleased(MouseButton::MIDDLE)) nk_input_button(
		nuklearkContext, NK_BUTTON_MIDDLE, mx, my, 0);

	if (inputManager->wasMousePressed(MouseButton::RIGHT)) nk_input_button(
		nuklearkContext, NK_BUTTON_RIGHT, mx, my, 1);
	if (inputManager->wasMouseReleased(MouseButton::RIGHT)) nk_input_button(
		nuklearkContext, NK_BUTTON_RIGHT, mx, my, 0);

	nk_input_scroll(nuklearkContext, nk_vec2(
		                static_cast<float>(inputManager->wheelDeltaX()),
		                static_cast<float>(inputManager->wheelDeltaY())
		                ));

	nk_input_key(nuklearkContext, NK_KEY_SHIFT,
	             inputManager->isKeyDown(KeyCode::LEFT_SHIFT) || inputManager->
	             isKeyDown(KeyCode::RIGHT_SHIFT));
	nk_input_key(nuklearkContext, NK_KEY_CTRL,
	             inputManager->isKeyDown(KeyCode::LEFT_CONTROL) || inputManager
	             ->isKeyDown(KeyCode::RIGHT_CONTROL));
	nk_input_key(nuklearkContext, NK_KEY_DEL, inputManager->isKeyDown(KeyCode::DELETE));
	nk_input_key(nuklearkContext, NK_KEY_ENTER, inputManager->isKeyDown(KeyCode::RETURN));
	nk_input_key(nuklearkContext, NK_KEY_TAB, inputManager->isKeyDown(KeyCode::TAB));
	nk_input_key(nuklearkContext, NK_KEY_BACKSPACE,
	             inputManager->isKeyDown(KeyCode::BACKSPACE));
	nk_input_key(nuklearkContext, NK_KEY_UP, inputManager->isKeyDown(KeyCode::UP_ARROW));
	nk_input_key(nuklearkContext, NK_KEY_DOWN,
	             inputManager->isKeyDown(KeyCode::DOWN_ARROW));
	nk_input_key(nuklearkContext, NK_KEY_LEFT,
	             inputManager->isKeyDown(KeyCode::LEFT_ARROW));
	nk_input_key(nuklearkContext, NK_KEY_RIGHT,
	             inputManager->isKeyDown(KeyCode::RIGHT_ARROW));

	bool ctrl = inputManager->isKeyDown(KeyCode::LEFT_CONTROL) || inputManager->
	            isKeyDown(KeyCode::RIGHT_CONTROL);
	nk_input_key(nuklearkContext, NK_KEY_COPY,
	             ctrl && inputManager->isKeyDown(KeyCode::C));
	nk_input_key(nuklearkContext, NK_KEY_PASTE,
	             ctrl && inputManager->isKeyDown(KeyCode::V));
	nk_input_key(nuklearkContext, NK_KEY_CUT,
	             ctrl && inputManager->isKeyDown(KeyCode::X));
	nk_input_key(nuklearkContext, NK_KEY_TEXT_UNDO,
	             ctrl && inputManager->isKeyDown(KeyCode::Z));
	nk_input_key(nuklearkContext, NK_KEY_TEXT_REDO,
	             ctrl && inputManager->isKeyDown(KeyCode::Y));
	nk_input_key(nuklearkContext, NK_KEY_TEXT_SELECT_ALL,
	             ctrl && inputManager->isKeyDown(KeyCode::A));
	nk_input_key(nuklearkContext, NK_KEY_TEXT_LINE_START,
	             inputManager->isKeyDown(KeyCode::HOME));
	nk_input_key(nuklearkContext, NK_KEY_TEXT_LINE_END,
	             inputManager->isKeyDown(KeyCode::END));

	nk_input_end(nuklearkContext);
}

void NuklearSDLRenderHook::beginFrame()
{
	updateInput();
	render();
}


void NuklearSDLRenderHook::render() const
{
	//Small example of Nuklear working
	//TODO Make this an actual canvas that can add and remove UIObjects.
	if (nk_begin(nuklearkContext, "Test Window", nk_rect(50, 50, 230, 250),
	             NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_dynamic(nuklearkContext, 30, 1);

		nk_layout_row_dynamic(nuklearkContext, 30, 2);
		static int option = 0;
		if (nk_option_label(nuklearkContext, "Easy", option == 0)) option = 0;
		if (nk_option_label(nuklearkContext, "Hard", option == 1)) option = 1;

		nk_layout_row_dynamic(nuklearkContext, 25, 1);
		static float value = 0.5f;
		nk_slider_float(nuklearkContext, 0, &value, 1.0f, 0.1f);
	}
	nk_end(nuklearkContext);
}

void NuklearSDLRenderHook::presentFrame()
{
	nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void NuklearSDLRenderHook::close()
{
	if (nuklearkContext) {
		nk_sdl_shutdown();
		nuklearkContext = nullptr;
	}
}