/**
 * @brief Abstract interface for UI rendering integration.
 *
 * Allows UI libraries (Nuklear, Dear ImGui, etc.) to hook into the renderer's
 * frame lifecycle without the renderer knowing which UI library is in use.
 */


#pragma once
#include "Component/UIObject/UIRenderCommand.h"

struct UserInterfaceSettings;

class IUIRenderHook
{
public:
	virtual ~IUIRenderHook() = default;

	/**
	 * @brief Sets up UI library resources.
	 *
	 * Called once after the renderer is fully initialized.
	 *
	 */
	virtual void initialize() = 0;

	/**
	 * @brief Feeds input state to the UI library.
	 *
	 * Called before beginFrame to ensure UI has current input state.
	 *
	 */
	virtual void updateInput() = 0;

	/**
	 * @brief Starts a new UI frame.
	 *
	 * Called at the start of each frame before any UI drawing.
	 *
	 */
	virtual void beginFrame() = 0;

	virtual void submit(UIRenderCommand command) = 0;
	/**
	 * @brief Submits UI draw commands to the renderer.
	 *
	 * Called after game rendering completes so UI appears on top.
	 * This is where the UI library's internal command buffer gets
	 * converted to actual draw calls.
	 *
	 */
	virtual void presentFrame() = 0;

	/**
	 * @brief Releases UI library resources.
	 *
	 * Must be called before the parent renderer is destroyed, since
	 * UI libraries typically hold references to graphics resources.
	 *
	 */
	virtual void close() = 0;
};