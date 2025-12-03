#pragma once

#include "Component.h"
#include "Rendering/RenderQueue/IRenderQueueWriter.h"
#include <optional>

class RenderComponent : public Component
{
public:
	~RenderComponent() = default;
	virtual void fillRenderQueue(IRenderQueueWriter& queue) const = 0;
};