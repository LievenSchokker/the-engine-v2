#pragma once
#include "Component/Camera.h"
#include "Rendering/RenderQueue/RenderQueue.h"

#include <optional>
#include <vector>


class WorldToCameraSpaceAdapter
{
public:
	static std::optional<RenderCommand> Transform(const Camera& camera, const RenderCommand& command);
	static bool IsInView(const Camera& camera, const RenderCommand& command,
						 float viewX, float viewY);
};


