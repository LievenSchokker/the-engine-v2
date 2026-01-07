#include "Rendering/viewport/CameraViewSpaceToScreenSpaceAdapter.h"

#include "Component/Transform.h"

RenderCommand CameraViewSpaceToScreenSpaceAdapter::Transform(
	const Camera& camera, const WindowOptions& window, const RenderCommand& command) {

	RenderCommand result = command;

	const float scaleX = window.width / camera.getViewWidth();
	const float scaleY = window.height / camera.getViewHeight();

	const float centerX = window.width / 2.0f;
	const float centerY = window.height / 2.0f;

	result.position.x = command.position.x * scaleX + centerX;
	result.position.y = command.position.y * scaleY + centerY;

	result.size.x = command.size.x * scaleX;
	result.size.y = command.size.y * scaleY;
	result.radius = command.radius * std::min(scaleX, scaleY);

	return result;
}

Vector2 CameraViewSpaceToScreenSpaceAdapter::findOptimalResolution(const Camera& camera, const WindowOptions& window)
{
	Vector2 resolutionScale {0,0};
	resolutionScale.x = window.width / camera.getViewWidth();
	resolutionScale.y = window.height / camera.getViewHeight();
	return resolutionScale;
}