#pragma once


#include "Component/Camera.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "Rendering/RenderQueue/RenderQueue.h"

#include <optional>
#include <vector>

class CameraViewSpaceToScreenSpaceAdapter
{
public:
	static RenderCommand Transform(const Camera& camera,
								const WindowOptions& viewport,
								const RenderCommand& command);
	static Vector2 findOptimalResolution(const Camera& camera,
										 const WindowOptions& viewport);
};