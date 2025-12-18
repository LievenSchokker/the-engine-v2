#include "Rendering/Nuklear/NuklearSDLRenderHook.h"

#include <algorithm>

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#include "../../../inc/Events/EventImplementations/ApplicationEvents.h"
#include "../../../inc/Events/EventImplementations/UserInterfaceEvent.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

#include <algorithm>
#include <iostream>
#include <algorithm>

NuklearSDLRenderHook::NuklearSDLRenderHook(SDL_Window* window,
                                           SDL_Renderer* renderer)
	: inputManager()
	  , sdlWindow(window)
	  , sdlRenderer(renderer)
	  , nuklearContext(nullptr),
	  eventDispatcher(nullptr),
	  mouseX(0),
	  mouseY(0)
{
}

NuklearSDLRenderHook::~NuklearSDLRenderHook()
{
	if (eventDispatcher != nullptr)
	{
		unSubscribeEvent(*eventDispatcher);
	}
}

void NuklearSDLRenderHook::initialize()
{
	nuklearContext = nk_sdl_init(sdlWindow, sdlRenderer);
	if (nuklearContext)
	{
		struct nk_font_atlas* atlas;
		nk_sdl_font_stash_begin(&atlas);
		nk_sdl_font_stash_end();

		nuklearContext->style.window.fixed_background = nk_style_item_color(
			nk_rgba(0, 0, 0, 255));
		nuklearContext->style.window.header.normal = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.header.hover = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
		nuklearContext->style.window.header.active = nk_style_item_color(
			nk_rgba(0, 0, 0, 0));
	}
}

void NuklearSDLRenderHook::setupEvents(EventDispatcher& dispatcher)
{
	eventDispatcher = &dispatcher;
	subscriptions.push_back(dispatcher.subscribe<MouseButtonPressedEvent>(
			[this](const MouseButtonPressedEvent& event)
			{
				this->handleMouseClick(event);
			}
			)
		);

	subscriptions.push_back(dispatcher.subscribe<MouseButtonReleasedEvent>(
			[this](const MouseButtonReleasedEvent& event)
			{
				this->handleMouseReleased(event);
			}
			)
		);

	subscriptions.push_back(dispatcher.subscribe<MouseMovedEvent>(
			[this](const MouseMovedEvent& event)
			{
				mouseX = event.x;
				mouseY = event.y;
			}
			)
		);
}

void NuklearSDLRenderHook::unSubscribeEvent(EventDispatcher& dispatcher)
{
	for (auto& handle : subscriptions)
	{
		dispatcher.unsubscribe(handle);
	}
	subscriptions.clear();
}

void NuklearSDLRenderHook::handleMouseClick(
	const MouseButtonPressedEvent& event)
{
	int idx = -1;
	switch (event.button)
	{
		case MouseButton::LEFT:
			idx = 0;
			break;
		case MouseButton::MIDDLE:
			idx = 1;
			break;
		case MouseButton::RIGHT:
			idx = 2;
			break;
	}
	if (idx >= 0)
	{
		pendingMouseDown[idx] = true;
		clickX[idx] = event.x;
		clickY[idx] = event.y;
	}
	switch (event.button)
	{
		case MouseButton::LEFT:
			nk_input_button(nuklearContext, NK_BUTTON_LEFT, event.x, event.y,
			                1);
			break;
		case MouseButton::MIDDLE:
			nk_input_button(nuklearContext, NK_BUTTON_MIDDLE, event.x, event.y,
			                1);
			break;
		case MouseButton::RIGHT:
			nk_input_button(nuklearContext, NK_BUTTON_RIGHT, event.x, event.y,
			                1);
			break;
	}
}

void NuklearSDLRenderHook::handleMouseReleased(
	const MouseButtonReleasedEvent& event)
{
	int idx = -1;
	switch (event.button)
	{
		case MouseButton::LEFT:
			idx = 0;
			break;
		case MouseButton::MIDDLE:
			idx = 1;
			break;
		case MouseButton::RIGHT:
			idx = 2;
			break;
	}
	if (idx >= 0)
	{
		pendingMouseDown[idx] = true;
		clickX[idx] = event.x;
		clickY[idx] = event.y;
	}

	switch (event.button)
	{
		case MouseButton::LEFT:
			nk_input_button(nuklearContext, NK_BUTTON_LEFT, event.x, event.y,
			                0);
			break;
		case MouseButton::MIDDLE:
			nk_input_button(nuklearContext, NK_BUTTON_MIDDLE, event.x, event.y,
			                0);
			break;
		case MouseButton::RIGHT:
			nk_input_button(nuklearContext, NK_BUTTON_RIGHT, event.x, event.y,
			                0);
			break;
	}
}

void NuklearSDLRenderHook::beginFrame()
{
	nk_input_begin(nuklearContext);
	nk_input_motion(nuklearContext, mouseX, mouseY);

	const nk_buttons buttons[] = {NK_BUTTON_LEFT, NK_BUTTON_MIDDLE,
	                              NK_BUTTON_RIGHT};
	for (int i = 0; i < 3; i++)
	{
		if (pendingMouseDown[i])
		{
			nk_input_button(nuklearContext, buttons[i], clickX[i], clickY[i],
			                1);
			pendingMouseDown[i] = false;
		}
		if (pendingMouseUp[i])
		{
			nk_input_button(nuklearContext, buttons[i], clickX[i], clickY[i],
			                0);
			pendingMouseUp[i] = false;
		}
	}

	commandQueue.clear();
	panelIndices.clear();
	panelElementIndices.clear();
	rootPanels.clear();
}

void NuklearSDLRenderHook::presentFrame()
{
	nk_input_end(nuklearContext);
	flushCommands();

	nk_sdl_render(NK_ANTI_ALIASING_ON);
	nk_clear(nuklearContext);
}

void NuklearSDLRenderHook::process(const std::vector<UIRenderCommand>& commands)
{
	for (auto command : commands)
	{
		commandQueue.push_back(std::move(command));
	}
}

void NuklearSDLRenderHook::flushCommands()
{
	std::stable_partition(commandQueue.begin(), commandQueue.end(),
	                      [](const UIRenderCommand& cmd)
	                      {
		                      return cmd.type == UICommandType::Panel;
	                      });

	const size_t originalSize = commandQueue.size();

	for (size_t i = 0; i < originalSize; ++i)
	{
		const auto& commandIterator = commandQueue[i];
		if (commandIterator.type == UICommandType::Panel)
		{
			panelIndices[commandIterator.panelId] = i;
			if (commandIterator.parentId == NO_PARENT)
			{
				rootPanels.push_back(commandIterator.panelId);
			}
		}
		else
		{
			if (panelIndices.find(commandIterator.panelId) == panelIndices.
			    end())
			{
				createDefaultPanel(commandIterator.panelId);
			}
			panelElementIndices[commandIterator.panelId].push_back(i);
		}
	}

	std::ranges::sort(rootPanels);
	rootPanels.erase(std::ranges::unique(rootPanels).begin(), rootPanels.end());

	for (uint32_t id : rootPanels)
	{
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
			break;

		case UICommandType::Text:
			renderText(command);
			break;
		case UICommandType::Button:
			renderButton(command);
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

		case UICommandType::Chart:
		{
			renderChart(command);
			break;
		}

		default:
			break;
	}
}

void NuklearSDLRenderHook::renderChart(const UIRenderCommand& command)
{
	if (nk_chart_begin(nuklearContext, NK_CHART_LINES, command.chartData.size(),
	                   command.chartMin, command.chartMax))
	{
		for (float value : command.chartData)
		{
			nk_chart_push(nuklearContext, value);
		}
		nk_chart_end(nuklearContext);
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

	nuklearContext->style.progress.cursor_normal =
		nk_style_item_color(barColor);
	nuklearContext->style.progress.cursor_hover = nk_style_item_color(barColor);
	nuklearContext->style.progress.cursor_active =
		nk_style_item_color(barColor);

	nk_size value = static_cast<nk_size>(command.progress * 100.0f);
	nk_size max = 100;
	nk_progress(nuklearContext, &value, max, NK_FIXED);

	nuklearContext->style.progress = originalStyle;
}

void NuklearSDLRenderHook::renderSeparator(const UIRenderCommand& command)
{
	nk_layout_row_dynamic(nuklearContext, 15, 1);

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

	// Consume the widget space
	nk_label(nuklearContext, "", NK_TEXT_LEFT);

	// Restore standard row height for following elements
	nk_layout_row_dynamic(nuklearContext, 20, 1);
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

void NuklearSDLRenderHook::renderButton(const UIRenderCommand& command)
{
	if (!command.interactable)
	{
		// Render disabled style
		nk_widget_disable_begin(nuklearContext);
	}

	// Apply custom colors
	struct nk_style_button originalStyle = nuklearContext->style.button;

	nuklearContext->style.button.normal = nk_style_item_color(
		nk_rgba(command.normalColor.r, command.normalColor.g,
		        command.normalColor.b, command.normalColor.a));
	nuklearContext->style.button.hover = nk_style_item_color(
		nk_rgba(command.hoverColor.r, command.hoverColor.g,
		        command.hoverColor.b, command.hoverColor.a));
	nuklearContext->style.button.active = nk_style_item_color(
		nk_rgba(command.pressedColor.r, command.pressedColor.g,
		        command.pressedColor.b, command.pressedColor.a));
	nuklearContext->style.button.text_normal =
		nk_rgba(command.textColor.r, command.textColor.g,
		        command.textColor.b, command.textColor.a);
	nuklearContext->style.button.text_hover = nuklearContext->style.button.
		text_normal;
	nuklearContext->style.button.text_active = nuklearContext->style.button.
		text_normal;

	if (nk_button_label(nuklearContext, command.text.c_str()))
	{
		if (command.interactable && eventDispatcher)
		{
			eventDispatcher->dispatch(UIButtonClickedEvent{command.buttonId});
		}
	}

	nuklearContext->style.button = originalStyle;

	if (!command.interactable)
	{
		nk_widget_disable_end(nuklearContext);
	}
}

void NuklearSDLRenderHook::close()
{
	if (nuklearContext)
	{
		nk_sdl_shutdown();
		nuklearContext = nullptr;
	}
}