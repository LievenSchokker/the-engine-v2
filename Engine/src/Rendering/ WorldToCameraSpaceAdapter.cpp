#include "Component/Transform.h"
#include "Rendering/ViewAdapters/WorldToCameraSpaceAdapter.h"
#include "Scene/Scene.h"

std::optional<RenderCommand> WorldToCameraSpaceAdapter::Transform(
	const Camera& camera, const RenderCommand& command)
{
	const Vector2 cameraPos = camera.getTransform()->getPosition();
	const Vector2 offset = camera.getOffset();
	const float zoom = camera.getZoom();

	const float viewX = command.position.x - (cameraPos.x + offset.x);
	const float viewY = command.position.y - (cameraPos.y + offset.y);

	if (!IsInView(camera, command, viewX, viewY))
	{
		return std::nullopt;
	}

	RenderCommand result = command;

	//IMPORTANT THIS IS NOT WINDOW HEIGHT
	const float centerX = camera.getViewWidth() / 2.0f;
	const float centerY = camera.getViewHeight() / 2.0f;

	// Apply zoom, center in view, then offset to viewport position
	result.position.x = (viewX * zoom) + centerX + camera.getViewportX();
	result.position.y = (viewY * zoom) + centerY + camera.getViewportY();

	result.size.x = command.size.x * zoom;
	result.size.y = command.size.y * zoom;
	result.radius = command.radius * zoom;

	return result;
}

bool WorldToCameraSpaceAdapter::IsInView(
	const Camera& camera, const RenderCommand& command,
	const float viewX, const float viewY)
{
	const float halfWidth = camera.getViewWidth() / 2.0f;
	const float halfHeight = camera.getViewHeight() / 2.0f;
	const float zoom = camera.getZoom();

	const float viewLeft = -halfWidth / zoom;
	const float viewRight = halfWidth / zoom;
	const float viewTop = -halfHeight / zoom;
	const float viewBottom = halfHeight / zoom;

	float objectRadius = 0.0f;
	objectRadius = std::max(command.size.x, command.size.y);


	//Is the object inside of the camera square Yess? Render it else no
	return viewX + objectRadius >= viewLeft &&
		   viewX - objectRadius <= viewRight &&
		   viewY + objectRadius >= viewTop &&
		   viewY - objectRadius <= viewBottom;
}