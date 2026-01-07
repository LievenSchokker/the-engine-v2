#include "../../inc/Rendering/viewport/WorldToCameraSpaceAdapter.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"

std::optional<RenderCommand> WorldToCameraSpaceAdapter::Transform(
	const Camera& camera, const RenderCommand& command)
{
	Vector2 cameraPos = camera.getTransform()
		? camera.getTransform()->getPosition()
		: Vector2::zero();

	const Vector2 offset = camera.getOffset();
	const float zoom = camera.getZoom();
	const float ppm = camera.getPixelsPerMeter();

	//Remove camera Position from world so everything is rendered relative to camera,
	const float viewX = command.position.x - (cameraPos.x + offset.x);
	const float viewY = command.position.y - (cameraPos.y + offset.y);

	if (!IsInView(camera, command, viewX, viewY))
	{
		return std::nullopt;
	}

	RenderCommand result = command;

	//Need to get center off screen otherwise camera would be at 0,0
	const float centerX = camera.getViewWidth() / 2.0f;
	const float centerY = camera.getViewHeight() / 2.0f;

	result.position.x = (viewX * ppm * zoom) + centerX + camera.getViewportX();
	result.position.y = (viewY * ppm * zoom) + centerY + camera.getViewportY();

	result.size.x = command.size.x * ppm * zoom;
	result.size.y = command.size.y * ppm * zoom;
	result.radius = command.radius * ppm * zoom;

	return result;
}

bool WorldToCameraSpaceAdapter::IsInView(
	const Camera& camera, const RenderCommand& command,
	const float viewX, const float viewY)
{
	const float ppm = camera.getPixelsPerMeter();
	const float zoom = camera.getZoom();

	const float halfWidthMeters = (camera.getViewWidth() / 2.0f) / (ppm * zoom);
	const float halfHeightMeters = (camera.getViewHeight() / 2.0f) / (ppm * zoom);

	float objectRadius = 0.0f;
	if (command.type == RenderCommandType::Circle) {
		objectRadius = command.radius;
	} else {
		objectRadius = std::max(command.size.x, command.size.y) / 2.0f;
	}

	return viewX + objectRadius >= -halfWidthMeters &&
		   viewX - objectRadius <= halfWidthMeters &&
		   viewY + objectRadius >= -halfHeightMeters &&
		   viewY - objectRadius <= halfHeightMeters;
}