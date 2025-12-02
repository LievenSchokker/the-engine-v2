// Rendering/Nuklear/NuklearSDLRenderHook.cpp
#include "Rendering/Nuklear/NuklearSDLRenderHook.h"

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

NuklearSDLRenderHook::NuklearSDLRenderHook(SDL_Window* window, SDL_Renderer* renderer)
    : inputManager(InputManager::getInstance())
    , sdlWindow(window)
    , sdlRenderer(renderer)
    , nuklearContext(nullptr) {
}

void NuklearSDLRenderHook::initialize() {
    nuklearContext = nk_sdl_init(sdlWindow, sdlRenderer);
    if (nuklearContext) {
        struct nk_font_atlas* atlas;
        nk_sdl_font_stash_begin(&atlas);
        nk_sdl_font_stash_end();

        // Transparent backgrounds
        nuklearContext->style.window.fixed_background = nk_style_item_color(nk_rgba(0, 0, 0, 0));
        nuklearContext->style.window.header.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
        nuklearContext->style.window.header.hover = nk_style_item_color(nk_rgba(0, 0, 0, 0));
        nuklearContext->style.window.header.active = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    }

	//This basicly prevents constant reallocation for small UI's
	//We might need to discuss if we care about custom memory solutions but for now this is fine
	commandQueue.reserve(256);
	rootPanels.reserve(16);
	panelIndices.reserve(32);
	panelElementIndices.reserve(32);

}

void NuklearSDLRenderHook::updateInput() {
    if (!nuklearContext || !inputManager) return;

    nk_input_begin(nuklearContext);

    const int mx = inputManager->mouseX();
    const int my = inputManager->mouseY();
    nk_input_motion(nuklearContext, mx, my);

    if (inputManager->wasMousePressed(MouseButton::LEFT))
        nk_input_button(nuklearContext, NK_BUTTON_LEFT, mx, my, 1);
    if (inputManager->wasMouseReleased(MouseButton::LEFT))
        nk_input_button(nuklearContext, NK_BUTTON_LEFT, mx, my, 0);

    if (inputManager->wasMousePressed(MouseButton::MIDDLE))
        nk_input_button(nuklearContext, NK_BUTTON_MIDDLE, mx, my, 1);
    if (inputManager->wasMouseReleased(MouseButton::MIDDLE))
        nk_input_button(nuklearContext, NK_BUTTON_MIDDLE, mx, my, 0);

    if (inputManager->wasMousePressed(MouseButton::RIGHT))
        nk_input_button(nuklearContext, NK_BUTTON_RIGHT, mx, my, 1);
    if (inputManager->wasMouseReleased(MouseButton::RIGHT))
        nk_input_button(nuklearContext, NK_BUTTON_RIGHT, mx, my, 0);

    nk_input_scroll(nuklearContext, nk_vec2(
        static_cast<float>(inputManager->wheelDeltaX()),
        static_cast<float>(inputManager->wheelDeltaY())
    ));

    nk_input_end(nuklearContext);
}

void NuklearSDLRenderHook::beginFrame() {
    updateInput();
	commandQueue.clear();
	panelIndices.clear();
	panelElementIndices.clear();
	rootPanels.clear();


	//TODO Remove hardcoded example
	UIRenderCommand panel;
	panel.type = UICommandType::Panel;
	panel.panelId = 1;
	panel.x = 50; panel.y = 50;
	panel.width = 200; panel.height = 150;
	panel.hasBorder = true;
	submit(panel);

	UIRenderCommand text;
	text.type = UICommandType::Text;
	text.panelId = 1;
	text.text = "Hello World";
	text.color = Color::white();
	submit(text);
}

void NuklearSDLRenderHook::presentFrame() {
    flushCommands();
    nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void NuklearSDLRenderHook::submit(UIRenderCommand command) {
	size_t index = commandQueue.size();
	commandQueue.push_back(std::move(command));

	const auto& cmd = commandQueue.back();

	if (cmd.type == UICommandType::Panel) {
		panelIndices[cmd.panelId] = index;
		if (cmd.parentId == NO_PARENT) {
			rootPanels.push_back(cmd.panelId);
		} else {
			panelElementIndices[cmd.parentId].push_back(index);
		}
	} else {
		panelElementIndices[cmd.panelId].push_back(index);
	}
}

void NuklearSDLRenderHook::flushCommands() {
	for (const auto& id : rootPanels) {
		renderPanel(id);
	}
}

void NuklearSDLRenderHook::renderElement(const UIRenderCommand& cmd) {
	switch (cmd.type) {
		case UICommandType::Panel:
			renderPanel(cmd.panelId);
			break;

		case UICommandType::Text: {
			nk_flags align = NK_TEXT_LEFT;
			if (cmd.alignment == Alignment::Center) align = NK_TEXT_CENTERED;
			else if (cmd.alignment == Alignment::Right) align = NK_TEXT_RIGHT;

			nk_color color = nk_rgba(cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
			nk_label_colored(nuklearContext, cmd.text.c_str(), align, color);
			break;
		}

		default:
			break;
	}
}

void NuklearSDLRenderHook::renderPanel(uint32_t panelId) {
	auto it = panelIndices.find(panelId);
	if (it == panelIndices.end()) return;

	const auto& panel = commandQueue[it->second];

	nk_flags flags = NK_WINDOW_NO_SCROLLBAR;
	if (panel.hasBorder) flags |= NK_WINDOW_BORDER;
	if (panel.hasTitle) flags |= NK_WINDOW_TITLE;

	char windowId[32];
	if (panel.title.empty()) {
		snprintf(windowId, sizeof(windowId), "##%u", panelId);
	}

	const char* name = panel.title.empty() ? windowId : panel.title.c_str();

	if (nk_begin(nuklearContext, name,
				 nk_rect(panel.x, panel.y, panel.width, panel.height), flags)) {

		nk_layout_row_dynamic(nuklearContext, panel.rowHeight, panel.columns);

		auto elemIt = panelElementIndices.find(panelId);
		if (elemIt != panelElementIndices.end()) {
			for (size_t idx : elemIt->second) {
				renderElement(commandQueue[idx]);
			}
		}
				 }
	nk_end(nuklearContext);
}

void NuklearSDLRenderHook::close() {
    if (nuklearContext) {
        nk_sdl_shutdown();
        nuklearContext = nullptr;
    }
}