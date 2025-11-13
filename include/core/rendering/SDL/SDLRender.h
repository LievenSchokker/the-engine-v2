/**
* @file SDLRender.h
 * @brief SDL2-based implementation of the rendering interface
 * @author Lieven Schokker
 * @date 11/11/2025
 */

/**
 * @class SDLRender
 * @brief SDL2 implementation of IRender for cross-platform windowing and rendering
 *
 * This class wraps SDL2's window and renderer API to provide a concrete implementation
 * of the IRender interface. SDL2 was chosen for its cross-platform compatibility and
 * hardware acceleration support.
 *
 *
 * @note Requires SDL video subsystem to be initialized via SdlContext
 * @see IRender, SdlContext
 */


#pragma once


#include <map>
#include <SDL.h>


#include "core/rendering/IRender.h"


class SdlContext;


class SDLRender : public IRender
{
public:
    /**
     * @brief Constructs an SDL renderer with dependency injection of SDL context
     *
     * Takes a reference to ensure SDL is already initialized and will remain valid
     * for the lifetime of this renderer. The assertion prevents runtime errors from
     * attempting to create windows without proper SDL initialization.
     *
     * @param context SDL context that must have video subsystem initialized
     * @pre context must have SDL_INIT_VIDEO initialized
     * @throws assertion failure if video subsystem not initialized (debug builds)
     */
    explicit SDLRender(SdlContext & context);

    /**
     * @brief Ensures proper cleanup of SDL resources in correct order
     *
     * Automatically calls close() to prevent resource leaks if the user forgets.
     */
    ~SDLRender() override;

    /**
     * @brief Creates SDL window with hardware-accelerated rendering
     *
     * Uses hardware acceleration and VSync to provide smooth, tear-free rendering.
     * The renderer is tightly coupled to the window to ensure they're destroyed
     * in the correct order (renderer first, then window) to avoid SDL errors.
     *
     * @param options Window configuration options
     * @note Errors are logged to stderr rather than throwing to allow graceful degradation
     */
    void open(const WindowOptions & options) override;

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

    /**
     * @brief Updates window title at runtime for dynamic feedback
     *
     * Allows changing the title without recreating the window, useful for
     * displaying, loading status, or other runtime information.
     *
     * @param title New window title text
     */
    void setTitle(const std::string & title) override;

    /**
     * @brief Presents the rendered frame with event processing and screen clearing
     *
     * Calls SDL_PumpEvents() to keep the window responsive to OS events (prevents
     * "not responding" dialogs).
     *
     * @note Returns early if renderer is invalid to prevent crashes
     */
    void presentFrame() override;

	void open();

private:
    SDL_Window* window = nullptr;      ///< Null indicates closed state; must outlive renderer
    SDL_Renderer* renderer = nullptr;  ///< Must be destroyed before window; null-checked for safety
    int windowWidth;                   ///< Cached to avoid repeated SDL queries
    int windowHeight;                  ///< Cached to avoid repeated SDL queries
};