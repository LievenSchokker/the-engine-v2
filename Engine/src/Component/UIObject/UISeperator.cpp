#include "Component/UIObject/UISeperator.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

UISeparator::UISeparator()
	: UIObject(0, 0, 0, 0)
{
}

void UISeparator::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
{
	if (!visible)
	{
		return;
	}

	UIRenderCommand command;
	command.type = UICommandType::Separator;
	command.panelId = panelId;
	command.parentId = parentId;

	queue.push(command);
}