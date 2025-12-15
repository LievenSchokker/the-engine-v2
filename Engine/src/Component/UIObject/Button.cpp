#include "Component/UIObject/Button.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

#include <iostream>

uint32_t Button::nextButtonId = 1;

Button::Button(
	float x,
	float y,
	float width,
	float height,
	std::string text
)
	: UIObject(x, y, width, height)
	, buttonId(nextButtonId++)
	, text(std::move(text))
{}

void Button::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
{
	if (!visible)
	{
		return;
	}

	UIRenderCommand command;
	command.type = UICommandType::Button;
	command.panelId = panelId;
	command.parentId = parentId;
	command.x = x;
	command.y = y;
	command.width = width;
	command.height = height;
	command.buttonId = buttonId;
	command.text = text;
	command.interactable = interactable;

	queue.push(command);
}

std::string Button::getText() const { return text; }
bool Button::isInteractable() const { return interactable; }
uint32_t Button::getButtonId() const { return buttonId; }

void Button::setText(const std::string& newText) { text = newText; }
void Button::setInteractable(bool enabled) { interactable = enabled; }