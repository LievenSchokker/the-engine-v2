#include "Component/UIElement/UIPanelElement.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

UIPanelElement::UIPanelElement(
	float x,
	float y,
	float width,
	float height,
	std::string title
	)
	: UIElement(x, y, width, height)
	  , title(std::move(title))
{
}

void UIPanelElement::fillUserInterfaceRenderQueue(
	IUserInterfaceRenderQueueWriter& queue) const
{
	if (!visible)
	{
		return;
	}

	UIRenderCommand command;
	command.type = UICommandType::Panel;
	command.panelId = panelId;
	command.parentId = parentId;
	command.x = x;
	command.y = y;
	command.width = width;
	command.height = height;
	command.title = title;
	command.hasTitle = !title.empty();
	command.hasBorder = border;
	command.rowHeight = rowHeight;
	command.columns = columns;
	queue.push(command);
}

std::string UIPanelElement::getTitle() const
{
	return title;
}

bool UIPanelElement::hasBorder() const
{
	return border;
}

bool UIPanelElement::hasTitle() const
{
	return !title.empty();
}

float UIPanelElement::getRowHeight() const
{
	return rowHeight;
}

int UIPanelElement::getColumns() const
{
	return columns;
}

void UIPanelElement::setTitle(const std::string& newTitle)
{
	title = newTitle;
}

void UIPanelElement::setBorder(bool enabled)
{
	border = enabled;
}

void UIPanelElement::setRowHeight(float height)
{
	rowHeight = height;
}

void UIPanelElement::setColumns(int cols)
{
	columns = cols;
}