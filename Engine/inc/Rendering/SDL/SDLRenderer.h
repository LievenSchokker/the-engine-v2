/**
 * @file SDLRenderer.h
 * @brief SDL2-based implementation of the rendering interface
 * @author Lieven Schokker
 * @date 11/11/2025
 */

/**
 * @class SDLRenderer
 * @brief SDL2 implementation of IRenderer for cross-platform windowing and
 * rendering
 *
 * This class wraps SDL2's window and renderer API to provide a concrete
 * implementation of the IRenderer interface. SDL2 was chosen for its
 * cross-platform compatibility and hardware acceleration support.
 *
 *
 * @note Requires SDL video subsystem to be initialized via SdlContext
 * @see IRenderer, SdlContext
 */


#pragma once

#include "External/IBackendContext.h"
#include "Rendering/IRenderer.h"
#include "Rendering/IUIRenderHook.h"
#include "Rendering/RenderCommand.h"

#include <SDL.h>
#include <memory>
#include <unordered_map>

class SDLRenderer: public IRenderer
{
   public:
	/**
	 * @brief Constructs an SDL renderer with dependency injection of SDL
	 * context
	 *
	 * Takes a reference to ensure SDL is already initialized and will remain
	 * valid for the lifetime of this renderer. The assertion prevents runtime
	 * errors from attempting to create windows without proper SDL
	 * initialization.
	 *
	 * @param context SDL context that must have video subsystem initialized
	 * @pre context must have SDL_INIT_VIDEO initialized
	 * @throws assertion failure if video subsystem not initialized (debug
	 * builds)
	 */
	explicit SDLRenderer(IBackendContext& context);

	/**
	 * @brief Ensures proper cleanup of SDL resources in correct order
	 *
	 * Automatically calls close() to prevent resource leaks if the user
	 * forgets.
	 */
	~SDLRenderer() override;

	/**
	 * @brief Creates SDL window with hardware-accelerated rendering
	 *
	 * Uses hardware acceleration and VSync to provide smooth, tear-free
	 * rendering. The renderer is tightly coupled to the window to ensure
	 * they're destroyed in the correct order (renderer first, then window) to
	 * avoid SDL errors.
	 *
	 * @param options Window configuration options
	 * @note Errors are logged to stderr rather than throwing to allow graceful
	 * degradation
	 */
	void open(const WindowOptions& options) override;

	/**
	 * @brief Clear the back buffer with a color.
	 */
	void beginFrame(const Color& clearColor) override;

	/**
	 * @brief Checks window existence as indicator of render system state
	 *
	 * Uses window pointer as the source of truth because the window must exist
	 * for the renderer to function. This simplifies state checking to a single
	 * null pointer check.
	 *
	 * @return true if window (and by extension, renderer) is valid
	 */
	bool isOpen() override;

	/**
	 * @brief Cleans up SDL resources in the required destruction order
	 *
	 * Destroys renderer before window because SDL requires child objects to be
	 * freed before their parents. Null checks prevent double-free crashes.
	 * Safe to call multiple times.
	 */
	void close() override;

	void execute(const RenderCommand& command) override;

	/**
	 * @brief Updates window title at runtime for dynamic feedback
	 *
	 * Allows changing the title without recreating the window, useful for
	 * displaying, loading status, or other runtime information.
	 *
	 * @param title New window title text
	 */
	void setTitle(const std::string& title) override;

	/**
	 * @brief Presents the rendered frame to the screen
	 *
	 * @note Returns early if renderer is invalid to prevent crashes
	 */
	void endFrame() override;

	void setUIRenderHook(std::unique_ptr<IUIRenderHook> hook) override;

	void submitUI(const std::vector<UIRenderCommand>& commands) override;

   private:
	/**
	 * @brief Draw a filled circle in window space.
	 */
	void drawCircle(const Vector2& center, double radius, const Color& color,
					const Vector2& scale);

	/**
	 * @brief Draw a filled rectangle in window space.
	 */
	void drawRectangle(const Vector2& center, const Vector2& size,
					   double rotationDegrees, const Color& color,
					   const Vector2& scale);

	/**
	 * @brief Draw a sprite from an image in window space.
	 */
	void drawSprite(const Vector2& position, const Vector2& size, IImage* image,
					const Rect* srcRect, double rotationDegrees,
					const Vector2& scale, const Color& tint, bool flipX = false,
					bool flipY = false);


   private:
	std::unique_ptr<IUIRenderHook> userInterfaceHook;
	void setupEvents(EventDispatcher& dispatcher) override;
	SDL_Texture* getOrCreateTexture(IImage* image);
	void clearTextureCache();
	bool ensureSolidQuadTexture();
	void destroySolidQuadTexture();
	SDL_Window* window =
		nullptr;  ///< Null indicates closed state; must outlive renderer
	SDL_Renderer* renderer =
		nullptr;  ///< Must be destroyed before window; null-checked for safety
	SDL_Texture* solidQuadTexture =
		nullptr;  ///< Texture for solid quad rendering
	std::unordered_map<IImage*, SDL_Texture*> textureCache;
	///< Cache of textures created from IImage surfaces
};
