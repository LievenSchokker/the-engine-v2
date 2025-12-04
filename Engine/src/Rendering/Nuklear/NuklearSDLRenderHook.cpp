// Rendering/Nuklear/NuklearSDLRenderHook.cpp
#include "Rendering/Nuklear/NuklearSDLRenderHook.h"

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

#include <iostream>

NuklearSDLRenderHook::NuklearSDLRenderHook(SDL_Window* window,
                                           SDL_Renderer* renderer)
	: inputManager(InputManager::getInstance())
	  , sdlWindow(window)
	  , sdlRenderer(renderer)
	  , nuklearContext(nullptr)
{
}

void NuklearSDLRenderHook::initialize()
{
	nuklearContext = nk_sdl_init(sdlWindow, sdlRenderer);
	if (nuklearContext)
	{
		struct nk_font_atlas* atlas;
		nk_sdl_font_stash_begin(&atlas);
		nk_sdl_font_stash_end();

		// Transparent backgrounds
		nuklearContext->style.window.fixed_background = nk_style_item_color(
			nk_rgba(0, 0, 0, 255));
		nuklearContext->style.window.header.normal = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.header.hover = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.header.active = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
	}

	//This basicly prevents constant reallocation for small UI's
	//We might need to discuss if we care about custom memory solutions but for now this is fine
	commandQueue.reserve(256);
	rootPanels.reserve(16);
	panelIndices.reserve(32);
	panelElementIndices.reserve(32);
}

void NuklearSDLRenderHook::updateInput()
{
	if (!nuklearContext || !inputManager) return;

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

	nk_input_end(nuklearContext);
}

void NuklearSDLRenderHook::beginFrame()
{
    updateInput();
    commandQueue.clear();
    panelIndices.clear();
    panelElementIndices.clear();
    rootPanels.clear();
}

void NuklearSDLRenderHook::presentFrame()
{
	flushCommands();
	nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void NuklearSDLRenderHook::process(const std::vector<UIRenderCommand>& commands)
{
	for (auto command : commands)
	{
		commandQueue.push_back(std::move(command));
	}
}


void NuklearSDLRenderHook::flushCommands() {
	/// Sort pannels so panels are always infront of the queue
	std::stable_partition(commandQueue.begin(), commandQueue.end(),
		[](const UIRenderCommand& cmd) {
			return cmd.type == UICommandType::Panel;
		});

	size_t originalSize = commandQueue.size();

	for (size_t i = 0; i < originalSize; ++i) {
		const auto& commandIterator = commandQueue[i];
		if (commandIterator.type == UICommandType::Panel) {
			panelIndices[commandIterator.panelId] = i;
			if (commandIterator.parentId == NO_PARENT) {
				rootPanels.push_back(commandIterator.panelId);
			} else {
				panelElementIndices[commandIterator.parentId].push_back(i);
			}
		} else {
			/// If no panelIndeices is found create default panel
			if (panelIndices.find(commandIterator.panelId) == panelIndices.end()) {
				createDefaultPanel(commandIterator.panelId);
			}
			panelElementIndices[commandIterator.panelId].push_back(i);
		}
	}

	for (uint32_t id : rootPanels) {
		renderPanel(id);
	}
}

void NuklearSDLRenderHook::createDefaultPanel(uint32_t panelId)
{
	//TODO estimate a panel size for each component that does not have panel yet
	UIRenderCommand panel;
	panel.type = UICommandType::Panel;
	panel.panelId = panelId;
	panel.parentId = NO_PARENT;
	panel.x = 0;
	panel.y = 0;
	panel.width = 200;
	panel.height = 200;
	panel.hasBorder = false;
	panel.hasTitle = false;
	panel.rowHeight = 30.0f;
	panel.columns = 1;

	size_t index = commandQueue.size();
	commandQueue.push_back(std::move(panel));
	panelIndices[panelId] = index;
	rootPanels.push_back(panelId);
}

void NuklearSDLRenderHook::renderElement(const UIRenderCommand& command)
{
	switch (command.type)
	{
		case UICommandType::Panel:
			renderPanel(command.panelId);
			break;

		case UICommandType::Text:
			renderText(command);
			break;

		case UICommandType::ProgressBar:
			renderProgressBar(command);
			break;

		case UICommandType::Separator:
			renderSeparator(command);
			break;

		case UICommandType::Spacer:
			renderSpacer(command);
			break;

		case UICommandType::Image:
			renderImage(command);
			break;

		default:
			break;
	}
}

void NuklearSDLRenderHook::renderProgressBar(const UIRenderCommand& command)
{
	// Save original style
	struct nk_style_progress originalStyle = nuklearContext->style.progress;

	// Convert our colors to nuklear colors
	nk_color barColor = nk_rgba(
		command.barColor.r,
		command.barColor.g,
		command.barColor.b,
		command.barColor.a
	);

	nk_color bgColor = nk_rgba(
		command.backgroundColor.r,
		command.backgroundColor.g,
		command.backgroundColor.b,
		command.backgroundColor.a
	);

	nuklearContext->style.progress.normal = nk_style_item_color(bgColor);
	nuklearContext->style.progress.hover = nk_style_item_color(bgColor);
	nuklearContext->style.progress.active = nk_style_item_color(bgColor);

	nuklearContext->style.progress.cursor_normal = nk_style_item_color(barColor);
	nuklearContext->style.progress.cursor_hover = nk_style_item_color(barColor);
	nuklearContext->style.progress.cursor_active = nk_style_item_color(barColor);

	nk_size value = static_cast<nk_size>(command.progress * 100.0f);
	nk_size max = 100;
	nk_progress(nuklearContext, &value, max, NK_FIXED);

	nuklearContext->style.progress = originalStyle;
}

void NuklearSDLRenderHook::renderSeparator(const UIRenderCommand& command)
{
	nk_layout_row_dynamic(nuklearContext, 2, 1);

	struct nk_rect bounds = nk_widget_bounds(nuklearContext);
	struct nk_command_buffer* canvas = nk_window_get_canvas(nuklearContext);

	nk_stroke_line(
		canvas,
		bounds.x,
		bounds.y + bounds.h / 2,
		bounds.x + bounds.w,
		bounds.y + bounds.h / 2,
		1.0f,
		nk_rgb(100, 100, 100)
	);

	nk_spacing(nuklearContext, 1);
}

void NuklearSDLRenderHook::renderImage(const UIRenderCommand& command)
{
	// TODO: Implement when texture/asset system
}
void NuklearSDLRenderHook::renderSpacer(const UIRenderCommand& command)
{
	nk_layout_row_dynamic(nuklearContext, command.spacerHeight, 1);
	nk_spacing(nuklearContext, 1);
}

void NuklearSDLRenderHook::renderText(const UIRenderCommand& command)
{
	nk_flags align = {};

	//set the alignment flag.
	if (command.alignment == Alignment::Center)
	{
		align = NK_TEXT_CENTERED;
	}
	else if (command.alignment == Alignment::Right)
	{
		align = NK_TEXT_RIGHT;
	}
	else
	{
		align = NK_TEXT_LEFT;
	}

	/// convert our color to nuklear color
	nk_color color = nk_rgba(
		command.color.r,
		command.color.g,
		command.color.b,
		command.color.a);

	/// Label is the nuklear equivalent of text.
	nk_label_colored(
		nuklearContext,
		command.text.c_str(),
		align,
		color);
}

void NuklearSDLRenderHook::renderPanel(uint32_t panelId)
{
	auto panelIterator = panelIndices.find(panelId);
	if (panelIterator == panelIndices.end()) return;

	const auto& panel = commandQueue[panelIterator->second];

	nk_flags flags = NK_WINDOW_NO_SCROLLBAR;
	if (panel.hasBorder) flags |= NK_WINDOW_BORDER;
	if (panel.hasTitle) flags |= NK_WINDOW_TITLE;

	char windowId[32];

	/// Always generate a pannel Id snprintf is a cheap call
	/// This is what this does btw:
	/// Composes a string with the same text that would be printed
	/// if format was used on printf, but instead of being printed,
	/// the content is stored as a C string in the buffer pointed
	/// by s (taking n as the maximum buffer capacity to fill).
	snprintf(windowId, sizeof(windowId), "##%u", panelId);

	/// Nuklear needs a pannel ID we use the title or if it's
	/// empty the generated one based on ID
	const char* name = panel.title.empty() ? windowId : panel.title.c_str();

	if (nk_begin(nuklearContext, name,
	             nk_rect(panel.x, panel.y, panel.width, panel.height), flags))
	{
		nk_layout_row_dynamic(nuklearContext, panel.rowHeight, panel.columns);

		//We render each element that belongs to panel!
		auto elementIterator = panelElementIndices.find(panelId);
		if (elementIterator != panelElementIndices.end())
		{
			for (size_t elementIndex : elementIterator->second)
			{
				renderElement(commandQueue[elementIndex]);
			}
		}
	}
	nk_end(nuklearContext);
}

void NuklearSDLRenderHook::close()
{
	if (nuklearContext)
	{
		nk_sdl_shutdown();
		nuklearContext = nullptr;
	}
}