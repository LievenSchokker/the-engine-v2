#pragma once
#include "Component/Camera.h"
#include "Rendering/RenderQueue/RenderQueue.h"

#include <vector>


class WorldToCameraSpaceAdapter
{
public:
	static RenderCommand RecalculateCommandWithCamera(const Camera& camera, RenderCommand& command);
};

