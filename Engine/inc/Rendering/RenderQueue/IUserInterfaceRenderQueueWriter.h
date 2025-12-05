#pragma once

#include "Component/BaseComponentTypes/UIRenderComponent.h"
#include "Rendering/RenderCommand.h"

/**
 * @brief Interface that makes sure components are only allowed to write to the
 * render queue. This makes sure they don't invalidate other data.
 * frame.
 */
class IUserInterfaceRenderQueueWriter
{
public :
	virtual ~IUserInterfaceRenderQueueWriter() = default;
	virtual void push(UIRenderCommand& command) = 0;
};