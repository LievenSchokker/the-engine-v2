#pragma once

#include "Input/InputManager.h"
#include "Rendering/IUIRenderHook.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "nuklear.h"

#include <SDL_video.h>
#include <queue>
#include <unordered_map>

struct PanelState;

/**
 * @brief SDL-specific implementation of Nuklear UI rendering.
 *
 * This hook bridges the engine's rendering pipeline with Nuklear's
 * immediate-mode UI. It exists as a separate class (rather than being built
 * into SDLRenderer) to keep UI concerns decoupled from core rendering, allowing
 * the UI library to be swapped without touching the renderer.
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
	~NuklearSDLRenderHook() override;

	/**
	 * @brief Sets up Nuklear context and fonts.
	 *
	 * Called once after the renderer is ready. Separated from the constructor
	 * because Nuklear initialization can fail, and we want construction to
	 * always succeed so the object can be safely destroyed.
	 */
	void initialize() override;
	void setupEvents(EventDispatcher& dispatcher) override;
	void unSubscribeEvent(EventDispatcher& dispatcher);
	void handleMouseClick(const MouseButtonPressedEvent& event);
	void handleMouseReleased(const MouseButtonReleasedEvent& event);

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
	void process(const std::vector<UIRenderCommand>& commands, InputManager& inputManager) override;
	void onResize(int width, int height) override;
private:
	int windowWidth = 0;
	int windowHeight = 0;

	void flushCommands();
	void createDefaultPanel(uint32_t panelId);
	void renderPanel(uint32_t panelId);
	void renderPanelContents(uint32_t panelId);
	void renderButton(const UIRenderCommand& command);
	void renderElement(const UIRenderCommand& command);
	void renderChart(const UIRenderCommand& command);
	void renderProgressBar(const UIRenderCommand& command);
	void renderSeparator(const UIRenderCommand& command);
	void renderImage(const UIRenderCommand& command);
	void renderSpacer(const UIRenderCommand& command);
	void renderText(const UIRenderCommand& command);
	void applyDocking(PanelState& state, const UIRenderCommand& panel);

	InputManager* inputManager;
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	nk_context* nuklearContext;
	EventDispatcher* eventDispatcher;
	
	bool pendingMouseDown[3] = {false, false, false};
	bool pendingMouseUp[3] = {false, false, false};
	int clickX[3] = {0, 0, 0};
	int clickY[3] = {0, 0, 0};
	int mouseX;
	int mouseY;


	std::unordered_map<uint32_t, PanelState> panelStateCache;

	std::vector<UIRenderCommand> commandQueue;

	/// @brief Maps panel ID → index in commandQueue where that panel's data lives.
	std::unordered_map<uint32_t, size_t> panelIndices;

	/// @brief Maps panel ID → list of indices of elements that belong to that panel.
	std::unordered_map<uint32_t, std::vector<size_t>> panelElementIndices;

	/// @brief list of index's that have no panel
	std::vector<uint32_t> rootPanels;

	std::vector<SubscriptionHandle> subscriptions;
};