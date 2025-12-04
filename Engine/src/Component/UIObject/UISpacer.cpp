#include "Component/UIObject/UISpacer.h"
#include "Rendering/UIRenderCommand.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"

UISpacer::UISpacer(float height)
	: UIObject(0, 0, 0, height)
	, spacerHeight(height)
{
}

void UISpacer::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
{
	if (!visible)
	{
		return;
	}

	UIRenderCommand command;
	command.type = UICommandType::Spacer;
	command.panelId = panelId;
	command.parentId = parentId;
	command.spacerHeight = spacerHeight;

	queue.push( command);
}

float UISpacer::getSpacerHeight() const { return spacerHeight; }
void UISpacer::setSpacerHeight(float newHeight) { spacerHeight = newHeight; }