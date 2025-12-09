#pragma once


#include "nuklear.h"

#include "Input/InputManager.h"
#include "Rendering/IUIRenderHook.h"
#include "Rendering/SDL/SDLRenderer.h"

#include <queue>
#include <unordered_map>
#include <SDL_video.h>

/**
 * @brief SDL-specific implementation of Nuklear UI rendering.
 *
 * This hook bridges the engine's rendering pipeline with Nuklear's immediate-mode UI.
 * It exists as a separate class (rather than being built into SDLRenderer) to keep
 * UI concerns decoupled from core rendering, allowing the UI library to be swapped
 * without touching the renderer.
 */
class NuklearSDLRenderHook: public IUIRenderHook
{
public:
	/**
	 * @brief Constructs the hook with SDL handles.
	 *
	 * Requires raw SDL pointers because Nuklear's SDL backend (nuklear_sdl_renderer.h)
	 * needs direct access to them for texture creation and rendering.
	 */
	NuklearSDLRenderHook(SDL_Window* window, SDL_Renderer* renderer);

	/**
	 * @brief Translates engine input state into Nuklear input events.
	 *
	 * This is a workaround for the fact that SDL events can only be polled once,
	 * but both InputManager and Nuklear need them. Rather than fighting over
	 * SDL_PollEvent, we let InputManager own event polling and then convert
	 * its processed state into Nuklear's expected format.
	 *
	 * @note This approach loses text input (SDL_TEXTINPUT events) since InputManager
	 *       only tracks key states, not character input. Text fields won't work
	 *       until InputManager gains text input support.
	 */
	void updateInput() override;

	/**
	 * @brief Sets up Nuklear context and fonts.
	 *
	 * Called once after the renderer is ready. Separated from the constructor
	 * because Nuklear initialization can fail, and we want construction to
	 * always succeed so the object can be safely destroyed.
	 */
	void initialize() override;

	/**
	 * @brief Processes input and builds UI for this frame.
	 *
	 * Called before game rendering so UI can react to input before the frame
	 * is presented. Nuklear is immediate-mode, so the entire UI must be
	 * rebuilt every frame.
	 */
	void beginFrame() override;

	/**
	 * @brief Defines the UI layout for this frame.
	 *
	 * This method build the main canvas too which elements can be added.
	 * Currently it contains some hard coded UI examples but after UIobject has been added
	 * This will render the whole canvas layer.
	 */
	void render();

	/**
	 * @brief Submits Nuklear draw commands to SDL.
	 *
	 * Called after all game rendering is complete so UI draws on top.
	 * This is where Nuklear's command buffer gets converted to actual
	 * SDL render calls.
	 */
	void presentFrame() override;

	/**
	 * @brief Releases Nuklear resources.
	 *
	 * Must be called before SDL_Renderer is destroyed, since Nuklear
	 * holds references to SDL textures.
	 */
	void close() override;

	/**
	* @brief submits a render command to the internal queue.
	*
	* Adds another command to the internal queue of the UIrenderer.
	* The whole queue will be flushed each frame. Expected usages is about 250 UI elements
	* More then that could slow down the engine since the internal queue has a
	* reserved size of 250 elements.
	*
	*/
	void process(const std::vector<UIRenderCommand>& commands) override;

private:
	void flushCommands();
	void createDefaultPanel(uint32_t panelId);
	void renderPanel(uint32_t panelId);
	void renderElement(const UIRenderCommand& command);
	void renderChart(const UIRenderCommand& command);
	void renderProgressBar(const UIRenderCommand& command);
	void renderSeparator(const UIRenderCommand& command);
	void renderImage(const UIRenderCommand& command);
	void renderSpacer(const UIRenderCommand& command);
	void renderText(const UIRenderCommand& command);

	InputManager* inputManager;
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	nk_context* nuklearContext;

	std::vector<UIRenderCommand> commandQueue;

	/// @brief Maps panel ID → index in commandQueue where that panel's data lives.
	std::unordered_map<uint32_t, size_t> panelIndices;

	/// @brief Maps panel ID → list of indices of elements that belong to that panel.
	std::unordered_map<uint32_t, std::vector<size_t>> panelElementIndices;

	/// @brief list of index's that have no panel
	std::vector<uint32_t> rootPanels;
};