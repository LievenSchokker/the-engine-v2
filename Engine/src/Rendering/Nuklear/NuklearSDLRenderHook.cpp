#include "Rendering/Nuklear/NuklearSDLRenderHook.h"

#include <iostream>
#include <ostream>

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#include "Component/UIObject/Alignment.h"
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
	  sdlRenderer(renderer), nuklearContext(nullptr)
{
}

void NuklearSDLRenderHook::initialize()
{
	nuklearContext = nk_sdl_init(sdlWindow, sdlRenderer);
	if (nuklearContext != nullptr)
	{
		struct nk_font_atlas* atlas;
		nk_sdl_font_stash_begin(&atlas);
		nk_sdl_font_stash_end();

		nuklearContext->style.window.fixed_background = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.fixed_background = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.header.normal = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.header.hover = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.header.active = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
	}
}

//Let me be  clear I DO NOT LIKE THIS.
//TODO Propper EventHandling so this can be fixed.
void NuklearSDLRenderHook::updateInput()
{
	if (!nuklearContext) return;
	if (!inputManager) return;

	nk_input_begin(nuklearContext);

	const int mx = inputManager->mouseX();
	const int my = inputManager->mouseY();
	nk_input_motion(nuklearContext, mx, my);

	if (inputManager->wasMousePressed(MouseButton::LEFT))
		nk_input_button(
			nuklearContext, NK_BUTTON_LEFT, mx, my, 1);
	if (inputManager->wasMouseReleased(MouseButton::LEFT))
		nk_input_button(
			nuklearContext, NK_BUTTON_LEFT, mx, my, 0);

	if (inputManager->wasMousePressed(MouseButton::MIDDLE))
		nk_input_button(
			nuklearContext, NK_BUTTON_MIDDLE, mx, my, 1);
	if (inputManager->wasMouseReleased(MouseButton::MIDDLE))
		nk_input_button(
			nuklearContext, NK_BUTTON_MIDDLE, mx, my, 0);

	if (inputManager->wasMousePressed(MouseButton::RIGHT))
		nk_input_button(
			nuklearContext, NK_BUTTON_RIGHT, mx, my, 1);
	if (inputManager->wasMouseReleased(MouseButton::RIGHT))
		nk_input_button(
			nuklearContext, NK_BUTTON_RIGHT, mx, my, 0);

	nk_input_scroll(nuklearContext, nk_vec2(
		                static_cast<float>(inputManager->wheelDeltaX()),
		                static_cast<float>(inputManager->wheelDeltaY())
		                ));

	nk_input_key(nuklearContext, NK_KEY_SHIFT,
	             inputManager->isKeyDown(KeyCode::LEFT_SHIFT) || inputManager->
	             isKeyDown(KeyCode::RIGHT_SHIFT));
	nk_input_key(nuklearContext, NK_KEY_CTRL,
	             inputManager->isKeyDown(KeyCode::LEFT_CONTROL) || inputManager
	             ->isKeyDown(KeyCode::RIGHT_CONTROL));
	nk_input_key(nuklearContext, NK_KEY_DEL,
	             inputManager->isKeyDown(KeyCode::DELETE));
	nk_input_key(nuklearContext, NK_KEY_ENTER,
	             inputManager->isKeyDown(KeyCode::RETURN));
	nk_input_key(nuklearContext, NK_KEY_TAB,
	             inputManager->isKeyDown(KeyCode::TAB));
	nk_input_key(nuklearContext, NK_KEY_BACKSPACE,
	             inputManager->isKeyDown(KeyCode::BACKSPACE));
	nk_input_key(nuklearContext, NK_KEY_UP,
	             inputManager->isKeyDown(KeyCode::UP_ARROW));
	nk_input_key(nuklearContext, NK_KEY_DOWN,
	             inputManager->isKeyDown(KeyCode::DOWN_ARROW));
	nk_input_key(nuklearContext, NK_KEY_LEFT,
	             inputManager->isKeyDown(KeyCode::LEFT_ARROW));
	nk_input_key(nuklearContext, NK_KEY_RIGHT,
	             inputManager->isKeyDown(KeyCode::RIGHT_ARROW));

	bool ctrl = inputManager->isKeyDown(KeyCode::LEFT_CONTROL) || inputManager->
	            isKeyDown(KeyCode::RIGHT_CONTROL);
	nk_input_key(nuklearContext, NK_KEY_COPY,
	             ctrl && inputManager->isKeyDown(KeyCode::C));
	nk_input_key(nuklearContext, NK_KEY_PASTE,
	             ctrl && inputManager->isKeyDown(KeyCode::V));
	nk_input_key(nuklearContext, NK_KEY_CUT,
	             ctrl && inputManager->isKeyDown(KeyCode::X));
	nk_input_key(nuklearContext, NK_KEY_TEXT_UNDO,
	             ctrl && inputManager->isKeyDown(KeyCode::Z));
	nk_input_key(nuklearContext, NK_KEY_TEXT_REDO,
	             ctrl && inputManager->isKeyDown(KeyCode::Y));
	nk_input_key(nuklearContext, NK_KEY_TEXT_SELECT_ALL,
	             ctrl && inputManager->isKeyDown(KeyCode::A));
	nk_input_key(nuklearContext, NK_KEY_TEXT_LINE_START,
	             inputManager->isKeyDown(KeyCode::HOME));
	nk_input_key(nuklearContext, NK_KEY_TEXT_LINE_END,
	             inputManager->isKeyDown(KeyCode::END));

	nk_input_end(nuklearContext);
}

void NuklearSDLRenderHook::beginFrame()
{
	updateInput();

	TextRenderCommand command1;
	command1.text = "Hello World";
	command1.x = 100;
	command1.y = 100;
	command1.width = 200;
	command1.height = 30;
	command1.color = Color::red();
	submit(command1);

	TextRenderCommand command2;
	command2.text = " World";
	command2.x = 300;
	command2.y = 300;
	command2.width = 200;
	command2.height = 30;
	submit(command2);
	render();
}


void NuklearSDLRenderHook::render()
{
	flushCommands();
}

void NuklearSDLRenderHook::flushCommands()
{
	for (const auto& cmd : commandQueue)
	{
		std::visit([this](const auto& c)
		{
			execute(c);
		}, cmd);
	}
	commandQueue.clear();
}

void NuklearSDLRenderHook::presentFrame()
{
	nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void NuklearSDLRenderHook::close()
{
	if (nuklearContext)
	{
		nk_sdl_shutdown();
		nuklearContext = nullptr;
	}
}

void NuklearSDLRenderHook::execute(const TextRenderCommand& cmd)
{
	char windowId[32];
	snprintf(windowId, sizeof(windowId), "##text_%d", commandCounter++);

	nk_flags flags = NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NO_INPUT |
	                 NK_WINDOW_BACKGROUND;

	if (nk_begin(nuklearContext, windowId,
	             nk_rect(cmd.x, cmd.y, cmd.width, cmd.height), flags))
	{
		nk_layout_row_dynamic(nuklearContext, cmd.height, 1);

		nk_flags align = NK_TEXT_LEFT;
		if (cmd.alignment == TextAlignment::Center) align = NK_TEXT_CENTERED;
		else if (cmd.alignment == TextAlignment::Right) align = NK_TEXT_RIGHT;

		nk_color color = nk_rgba(
			cmd.color.r,
			cmd.color.g,
			cmd.color.b,
			cmd.color.a
			);

		nk_label_colored(nuklearContext, cmd.text.c_str(), align, color);
	}
	nk_end(nuklearContext);
}

void NuklearSDLRenderHook::submit(UIRenderCommand command)
{
	commandQueue.push_back(std::move(command));
}

void NuklearSDLRenderHook::execute(const ButtonRenderCommand& cmd)
{
	char windowId[32];
	snprintf(windowId, sizeof(windowId), "##btn_%d", commandCounter++);

	nk_flags flags = NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND;

	if (nk_begin(nuklearContext, windowId,
	             nk_rect(cmd.x, cmd.y, cmd.width, cmd.height), flags))
	{
		nk_layout_row_dynamic(nuklearContext, cmd.height - 8, 1);

		if (nk_button_label(nuklearContext, cmd.label.c_str()))
		{
			if (cmd.enabled && cmd.wasClicked)
			{
				*cmd.wasClicked = true;
			}
		}
	}
	nk_end(nuklearContext);
}