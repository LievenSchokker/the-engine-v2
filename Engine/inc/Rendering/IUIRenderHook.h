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


struct UserInterfaceSettings;

class IUIRenderHook
{
   public:
	virtual ~IUIRenderHook() = default;

	/**
	 * @brief Opens a window with the specified options
	 *
	 * Creates and initializes a window with the properties defined in the
	 * WindowOptions parameter.
	 *
	 * @throws May throw implementation-specific exceptions on failure
	 *
	 */
	virtual void initialize() = 0;


    virtual void updateInput() = 0;
	/**
	 * @brief Clears the current back buffer with the supplied color.
	 *
	 */
	virtual void beginFrame() = 0;

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



};
