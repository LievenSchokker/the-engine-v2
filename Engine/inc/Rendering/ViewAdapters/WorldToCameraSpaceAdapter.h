#pragma once


#include "Component/Camera.h"
#include "Rendering/RenderQueue/RenderQueue.h"

#include <optional>
#include <vector>

/**
 * @class WorldToCameraSpaceAdapter
 * @brief Transforms render commands from world space to camera/view space.
 *
 * This utility class handles the conversion of world-space positions to
 * view-space positions relative to a camera. It also performs frustum culling
 * to skip rendering objects outside the camera's view.
 *
 * This class has been mainly designed with the idea of a one camera viewport
 *
 */
class WorldToCameraSpaceAdapter
{
public:
	/**
	 * @brief Transforms a render command from world space to camera view space.
	 *
	 * Applies camera position, offset, zoom, and viewport positioning to
	 * convert the command's world-space coordinates to screen-space coordinates.
	 * Also performs culling to skip objects outside the camera's view.
	 *
	 * @param camera The camera
	 * @param command The render command in world space
	 * @return The transformed render command, or std::nullopt if culled
	 * (outside of camera view)
	 *
	 * @see IsInView() for culling logic
	 */
	static std::optional<RenderCommand> Transform(
		const Camera& camera,
		const RenderCommand& command);

private:
	/**
	* @brief Checks if a render command is within the camera's view bounds.
	*
	* This does camera culling on a very basic level, It checks if the items are inside of
	* the camera view if they it return false.
	* Now importantly this system doesn't really work nicely for two camera's
	* since it either renders the item or doesn't (so it does not cleanly cutt off)
	*
	*/
	static bool IsInView(
		const Camera& camera,
		const RenderCommand& command,
		float viewX,
		float viewY);
};