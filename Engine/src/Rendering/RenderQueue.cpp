#include "Rendering/RenderQueue/RenderQueue.h"

void RenderQueue::push(RenderCommand command)
{
	worldQueue.push(command);
}

void RenderQueue::push(UIRenderCommand command)
{
	uiQueue.push(command);
}

void RenderQueue::sortAll()
{
	worldQueue.sort();
	uiQueue.sort();
}

void RenderQueue::clearAll()
{
	worldQueue.clear();
	uiQueue.clear();
}

RenderQueueBase<RenderCommand>& RenderQueue::world()
{
	return worldQueue;
}

RenderQueueBase<UIRenderCommand>& RenderQueue::ui()
{
	return uiQueue;
}