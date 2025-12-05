#include "Component/UIObject/UITextObject.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

UITextObject::UITextObject(
	float x,
	float y,
	float width,
	float height,
	std::string text,
	int fontSize,
	Alignment alignment,
	Color color
)
	: UIObject(x, y, width, height)
	, text(std::move(text))
	, fontSize(fontSize)
	, alignment(alignment)
	, color(color)
{
}

void UITextObject::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
{
	if (!visible)
	{
		return;
	}

	UIRenderCommand command;
	command.type = UICommandType::Text;
	command.panelId = panelId;
	command.parentId = parentId;
	command.x = x;
	command.y = y;
	command.width = width;
	command.height = height;
	command.text = text;
	command.fontSize = fontSize;
	command.alignment = alignment;
	command.color = color;

	queue.push(command);
}

std::string UITextObject::getText() const { return text; }
int UITextObject::getFontSize() const { return fontSize; }
Alignment UITextObject::getAlignment() const { return alignment; }
Color UITextObject::getColor() const { return color; }

void UITextObject::setText(const std::string& newText) { text = newText; }
void UITextObject::setFontSize(int newSize) { fontSize = newSize; }
void UITextObject::setAlignment(Alignment newAlignment) { alignment = newAlignment; }
void UITextObject::setColor(const Color& newColor) { color = newColor; }