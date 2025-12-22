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
	: inputManager(nullptr)
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
		case MouseButton::LEFT:   idx = 0; break;
		case MouseButton::MIDDLE: idx = 1; break;
		case MouseButton::RIGHT:  idx = 2; break;
	}
	if (idx >= 0)
	{
		pendingMouseDown[idx] = true;
		clickX[idx] = event.x;
		clickY[idx] = event.y;
	}
}

void NuklearSDLRenderHook::handleMouseReleased(
	const MouseButtonReleasedEvent& event)
{
	int idx = -1;
	switch (event.button)
	{
		case MouseButton::LEFT:   idx = 0; break;
		case MouseButton::MIDDLE: idx = 1; break;
		case MouseButton::RIGHT:  idx = 2; break;
	}
	if (idx >= 0)
	{
		pendingMouseUp[idx] = true;
		clickX[idx] = event.x;
		clickY[idx] = event.y;
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
}

void NuklearSDLRenderHook::process(const std::vector<UIRenderCommand>& commands, InputManager& inputManager)
{
	this->inputManager = &inputManager;
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

    PanelState& state = panelStateCache[panelId];
    if (!state.initialized) {
        state.x = panel.x;
        state.y = panel.y;
        state.width = panel.width;
        state.height = panel.height;
        state.isClosed = false;
        state.initialized = true;
    }

    if (state.isClosed) {
        return;
    }

    nk_flags flags = 0;
    if (panel.hasBorder) flags |= NK_WINDOW_BORDER;
    if (panel.hasTitle) flags |= NK_WINDOW_TITLE;
    if (!panel.scrollable) flags |= NK_WINDOW_NO_SCROLLBAR;
    if (panel.resizable) flags |= NK_WINDOW_SCALABLE;
    if (panel.minimizable) flags |= NK_WINDOW_MINIMIZABLE;
    if (panel.closable) flags |= NK_WINDOW_CLOSABLE;
    if (panel.movable) flags |= NK_WINDOW_MOVABLE;

    char windowId[32];
    snprintf(windowId, sizeof(windowId), "##%u", panelId);

    struct nk_style_window originalStyle = nuklearContext->style.window;

    // ... all your styling code stays the same ...
    nuklearContext->style.window.fixed_background = nk_style_item_color(
        nk_rgba(panel.backgroundColor.r, panel.backgroundColor.g,
                panel.backgroundColor.b, panel.backgroundColor.a));
    nuklearContext->style.window.border_color =
        nk_rgba(panel.borderColor.r, panel.borderColor.g,
                panel.borderColor.b, panel.borderColor.a);
    nuklearContext->style.window.border = panel.borderThickness;
    nuklearContext->style.window.rounding = panel.rounding;
    nuklearContext->style.window.padding = nk_vec2(panel.padding, panel.padding);
    nuklearContext->style.window.spacing = nk_vec2(panel.spacing, panel.spacing);

    nuklearContext->style.window.header.normal = nk_style_item_color(
        nk_rgba(panel.titleBackgroundColor.r, panel.titleBackgroundColor.g,
                panel.titleBackgroundColor.b, panel.titleBackgroundColor.a));
    nuklearContext->style.window.header.hover =
        nuklearContext->style.window.header.normal;
    nuklearContext->style.window.header.active =
        nuklearContext->style.window.header.normal;

    nuklearContext->style.window.header.label_normal =
        nk_rgba(panel.titleTextColor.r, panel.titleTextColor.g,
                panel.titleTextColor.b, panel.titleTextColor.a);
    nuklearContext->style.window.header.label_hover =
        nuklearContext->style.window.header.label_normal;
    nuklearContext->style.window.header.label_active =
        nuklearContext->style.window.header.label_normal;

    nuklearContext->style.window.header.padding =
        nk_vec2(panel.titlePadding, panel.titlePadding);
    nuklearContext->style.window.header.label_padding =
        nk_vec2(panel.titleLabelPadding, panel.titleLabelPadding);

    const char* name = panel.title.empty() ? windowId : panel.title.c_str();

    // DON'T set position/size every frame - only on first creation
    // Use nk_begin_titled to separate display title from ID
    struct nk_window* win = nk_window_find(nuklearContext, name);

    if (win == nullptr) {
        // First time - create with our initial bounds
        if (nk_begin(nuklearContext, name,
                     nk_rect(state.x, state.y, state.width, state.height), flags))
        {
            nk_layout_row_dynamic(nuklearContext, panel.rowHeight, panel.columns);
            renderPanelContents(panelId);
        }
    } else {
        // Window exists - let Nuklear use its cached state
        if (nk_begin(nuklearContext, name,
                     nk_rect(win->bounds.x, win->bounds.y,
                             win->bounds.w, win->bounds.h), flags))
        {
            nk_layout_row_dynamic(nuklearContext, panel.rowHeight, panel.columns);
            renderPanelContents(panelId);
        }
    }

    // Update our cache from Nuklear's actual state
    struct nk_rect bounds = nk_window_get_bounds(nuklearContext);
    state.x = bounds.x;
    state.y = bounds.y;
    state.width = bounds.w;
    state.height = bounds.h;

    // Check if window was closed
    if (panel.closable && nk_window_is_hidden(nuklearContext, name)) {
        state.isClosed = true;
    }

    nk_end(nuklearContext);
    nuklearContext->style.window = originalStyle;
}

// Helper to avoid code duplication
void NuklearSDLRenderHook::renderPanelContents(uint32_t panelId)
{
    auto elementIterator = panelElementIndices.find(panelId);
    if (elementIterator != panelElementIndices.end())
    {
        for (size_t elementIndex : elementIterator->second)
        {
            renderElement(commandQueue[elementIndex]);
        }
    }
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
		nk_free(nuklearContext);
		nuklearContext = nullptr;
	}
}