#include "Component/Transform.h"
#include "Rendering/viewport/WorldToCameraViewSpaceAdapter.h"

RenderCommand WorldToCameraViewSpaceAdapter::Transform(
	const Camera& camera, const RenderCommand& command) {

	Vector2 cameraPos = Vector2::zero();
	if (camera.getTransform() != nullptr) {
		cameraPos = camera.getTransform()->getPosition();
	}

	const Vector2 offset = camera.getOffset();
	const float zoom = camera.getZoom();

	const float viewX = command.position.x - (cameraPos.x + offset.x);
	const float viewY = command.position.y - (cameraPos.y + offset.y);

	RenderCommand result = command;

	result.position.x = viewX * zoom;
	result.position.y = viewY * zoom;
	result.size.x = command.size.x * zoom;
	result.size.y = command.size.y * zoom;
	result.radius = command.radius * zoom;

	return result;
}

