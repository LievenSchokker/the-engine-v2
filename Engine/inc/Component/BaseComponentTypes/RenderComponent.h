#pragma once


#include "Component.h"
#include "Rendering/RenderQueue/IRenderQueueWriter.h"

class RenderComponent : virtual public Component
{
public:
	~RenderComponent() override = default;
	virtual void fillRenderQueue(IRenderQueueWriter& queue) const = 0;
};