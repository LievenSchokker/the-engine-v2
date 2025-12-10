#include "Component/UIElement/UITextElement.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

UITextElement::UITextElement(
	float x,
	float y,
	float width,
	float height,
	std::string text,
	int fontSize,
	Alignment alignment,
	Color color
)
	: UIElement(x, y, width, height)
	, text(std::move(text))
	, fontSize(fontSize)
	, alignment(alignment)
	, color(color)
{
}

void UITextElement::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
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

std::string UITextElement::getText() const { return text; }
int UITextElement::getFontSize() const { return fontSize; }
Alignment UITextElement::getAlignment() const { return alignment; }
Color UITextElement::getColor() const { return color; }

void UITextElement::setText(const std::string& newText) { text = newText; }
void UITextElement::setFontSize(int newSize) { fontSize = newSize; }
void UITextElement::setAlignment(Alignment newAlignment) { alignment = newAlignment; }
void UITextElement::setColor(const Color& newColor) { color = newColor; }