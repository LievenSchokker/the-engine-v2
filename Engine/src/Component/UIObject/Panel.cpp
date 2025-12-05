#include "Component/UIObject/UIPanelObject.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

UIPanelObject::UIPanelObject(
	float x,
	float y,
	float width,
	float height,
	std::string title
)
	: UIObject(x, y, width, height)
	, title(std::move(title))
{
}

void UIPanelObject::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
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

std::string UIPanelObject::getTitle() const { return title; }
bool UIPanelObject::hasBorder() const { return border; }
bool UIPanelObject::hasTitle() const { return !title.empty(); }
float UIPanelObject::getRowHeight() const { return rowHeight; }
int UIPanelObject::getColumns() const { return columns; }

void UIPanelObject::setTitle(const std::string& newTitle) { title = newTitle; }
void UIPanelObject::setBorder(bool enabled) { border = enabled; }
void UIPanelObject::setRowHeight(float height) { rowHeight = height; }
void UIPanelObject::setColumns(int cols) { columns = cols; }