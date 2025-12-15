#pragma once

#include "Rendering/RenderCommand.h"

/**
 * @brief Interface that makes sure components are only allowed to write to the
 * render queue. This makes sure they don't invalidate other data.
 * frame.
 */
class IRenderQueueWriter
{
public :
	virtual ~IRenderQueueWriter() = default;
	virtual void push(RenderCommand command) = 0;
};