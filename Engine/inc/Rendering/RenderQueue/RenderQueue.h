#pragma once

#include "RenderQueueBase.h"
#include "IRenderQueueWriter.h"
#include "IUserInterfaceRenderQueueWriter.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/UIRenderCommand.h"

class RenderQueue : public IRenderQueueWriter,
					public IUserInterfaceRenderQueueWriter
{
public:
	void push(RenderCommand& command) override;
	void push(UIRenderCommand& command) override;

	void sortAll();
	void clearAll();

	RenderQueueBase<RenderCommand>& world();
	RenderQueueBase<UIRenderCommand>& ui();

private:
	RenderQueueBase<RenderCommand> worldQueue;
	RenderQueueBase<UIRenderCommand> uiQueue;
};