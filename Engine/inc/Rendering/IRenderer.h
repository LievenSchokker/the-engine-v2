/**
 * @file IRenderer.h
 * @brief Interface for rendering system implementations
 */

/**
 * @interface IRenderer
 * @brief Abstract interface for window and rendering management
 *
 * This interface defines the contract for rendering system implementations.
 * It provides methods for window lifecycle management (open, close), frame
 * presentation, and window property manipulation.
 *
 * This interface creates an abstraction layer between IRenderer and the actual
 * rendering implementation In case the decision is later made to use another
 * rendering API.
 *
 */

#pragma once

class string;
struct WindowOptions;

#include "../Math/Vector2.h"
#include "Color.h"

#include <string>

class IRenderer
{
   public:
	virtual ~IRenderer() = default;

	/**
	 * @brief Opens a window with the specified options
	 *
	 * Creates and initializes a window with the properties defined in the
	 * WindowOptions parameter.
	 *
	 * @param opts Window configuration options
	 * @throws May throw implementation-specific exceptions on failure
	 *
	 */
	virtual void open(const WindowOptions& opts) = 0;

	/**
	 * @brief Clears the current back buffer with the supplied color.
	 *
	 * @param clearColor Color used while clearing.
	 */
	virtual void beginFrame(const Color& clearColor) = 0;

	/**
	 * @brief Presents the current frame to the screen
	 *
	 *
	 * @pre Window must be open
	 * @see open()
	 */
	virtual void presentFrame() = 0;

	/**
	 * @brief Closes the rendering window and releases associated resources
	 *
	 * Destroys the window and cleans up rendering resources. After calling
	 * this method, isOpen() should return false.
	 *
	 * @note Safe to call on an already closed window (should be a no-op)
	 * @see open(), isOpen()
	 */
	virtual void close() = 0;

	/**
	 * @brief Checks if the rendering window is currently open
	 *
	 * @return true if the window is open and ready for rendering
	 * @return false if the window is closed or not yet created
	 * @see open(), close()
	 */
	virtual bool isOpen() = 0;

	/**
	 * @brief Sets the window title text
	 *
	 * @param title The new title text to display
	 * @pre Window should be open (behavior when closed is
	 * implementation-defined)
	 * @see open()
	 */
	virtual void setTitle(const std::string& title) = 0;

	/**
	 * @brief Draw a filled circle with the given configuration.
	 */
	virtual void drawCircle(const Vector2& center, double radius,
							const Color& color, const Vector2& scale) = 0;

	/**
	 * @brief Draw a filled rectangle with the given configuration.
	 */
	virtual void drawRectangle(const Vector2& center, const Vector2& size,
							   double rotationDegrees, const Color& color,
							   const Vector2& scale) = 0;
};