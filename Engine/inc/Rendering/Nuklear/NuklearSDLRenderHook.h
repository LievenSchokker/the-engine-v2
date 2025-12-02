#pragma once

#include <SDL_video.h>

#include "Input/InputManager.h"
#include "Rendering/IUIRenderHook.h"
#include "Rendering/SDL/SDLRenderer.h"

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
	void render() const;

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

private:
	/// Cached to avoid repeated singleton lookups each frame.
	InputManager* inputManager;

	/// Needed by Nuklear for window size queries and event context.
	SDL_Window* sdlWindow;

	/// Needed by Nuklear for texture and draw call submission.
	SDL_Renderer* sdlRenderer;

	/// Nuklear's core state. Null until initialize() succeeds.
	struct nk_context* nuklearkContext;
};