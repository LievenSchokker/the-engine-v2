#include "Component/UIElement/UIPanelElement.h"

#include "Core/GameWorld.h"
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

void UIPanelElement::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
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
	command.backgroundColor = backgroundColor;

	// Visual styling
	command.borderColor = borderColor;
	command.borderThickness = borderThickness;
	command.padding = padding;
	command.spacing = spacing;

	// Title customization
	command.titleBackgroundColor = titleBackgroundColor;
	command.titleTextColor = titleTextColor;
	command.titleHeight = titleHeight;
	command.titlePadding = titlePadding;
	command.titleLabelPadding = titleLabelPadding;

	// Behavior
	command.scrollable = scrollable;
	command.resizable = resizable;
	command.minimizable = minimizable;
	command.closable = closable;

	queue.push(command);
}

std::string UIPanelElement::getTitle() const { return title; }
bool UIPanelElement::hasBorder() const { return border; }
bool UIPanelElement::hasTitle() const { return !title.empty(); }
float UIPanelElement::getRowHeight() const { return rowHeight; }
int UIPanelElement::getColumns() const { return columns; }

void UIPanelElement::setTitle(const std::string& newTitle) { title = newTitle; }
void UIPanelElement::setBorder(bool enabled) { border = enabled; }
void UIPanelElement::setRowHeight(float height) { rowHeight = height; }
void UIPanelElement::setColumns(int cols) { columns = cols; }
void UIPanelElement::setBackgroundColor(Color color) { backgroundColor = color; }

// UIPanelElement.cpp

// Visual Styling
Color UIPanelElement::getBorderColor() const { return borderColor; }
float UIPanelElement::getBorderThickness() const { return borderThickness; }
float UIPanelElement::getPadding() const { return padding; }
float UIPanelElement::getSpacing() const { return spacing; }

void UIPanelElement::setBorderColor(Color color) { borderColor = color; }
void UIPanelElement::setBorderThickness(float thickness) { borderThickness = thickness; }
void UIPanelElement::setPadding(float pad) { padding = pad; }
void UIPanelElement::setSpacing(float space) { spacing = space; }

// Title Customization
Color UIPanelElement::getTitleBackgroundColor() const { return titleBackgroundColor; }
Color UIPanelElement::getTitleTextColor() const { return titleTextColor; }

void UIPanelElement::setTitleBackgroundColor(Color color) { titleBackgroundColor = color; }
void UIPanelElement::setTitleTextColor(Color color) { titleTextColor = color; }

// Behavior
bool UIPanelElement::isScrollable() const { return scrollable; }
bool UIPanelElement::isResizable() const { return resizable; }
bool UIPanelElement::isMinimizable() const { return minimizable; }
bool UIPanelElement::isClosable() const { return closable; }

void UIPanelElement::setScrollable(bool enabled) { scrollable = enabled; }
void UIPanelElement::setResizable(bool enabled) { resizable = enabled; }
void UIPanelElement::setMinimizable(bool enabled) { minimizable = enabled; }
void UIPanelElement::setClosable(bool enabled) { closable = enabled; }

float UIPanelElement::getTitleHeight() const { return titleHeight; }
float UIPanelElement::getTitlePadding() const { return titlePadding; }
float UIPanelElement::getTitleLabelPadding() const { return titleLabelPadding; }

void UIPanelElement::setTitleHeight(float height) { titleHeight = height; }
void UIPanelElement::setTitlePadding(float pad) { titlePadding = pad; }
void UIPanelElement::setTitleLabelPadding(float pad) { titleLabelPadding = pad; }