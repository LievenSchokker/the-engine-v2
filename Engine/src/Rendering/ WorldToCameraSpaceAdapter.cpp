#include "Component/Transform.h"
#include "Rendering/ViewAdapters/WorldToCameraSpaceAdapter.h"
#include "Scene/Scene.h"



RenderCommand WorldToCameraSpaceAdapter::RecalculateCommandWithCamera(
	const Camera& camera, RenderCommand& command)
{
	Vector2 cameraPosition = camera.getTransform()->getPosition();
	command.position = cameraPosition - command.position;
	//command.position.x += camera.getOffsetX();
	//command.position.y += camera.getOffsetY();
	return command;
}