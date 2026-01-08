#include "Component/UIElement/UIButton.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

#include <iostream>

uint32_t UIButton::nextButtonId = 1;

UIButton::UIButton(
	float x,
	float y,
	float width,
	float height,
	std::string text
)
	: UIElement(x, y, width, height)
	, buttonId(nextButtonId++)
	, text(std::move(text))
{}

void UIButton::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
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
	command.normalColor = normalColor;
	command.hoverColor = hoverColor;
	command.pressedColor = pressedColor;
	command.textColor = textColor;

	queue.push(command);
}

std::string UIButton::getText() const { return text; }
bool UIButton::isInteractable() const { return interactable; }
uint32_t UIButton::getButtonId() const { return buttonId; }

void UIButton::setText(const std::string& newText) { text = newText; }
void UIButton::setInteractable(bool enabled) { interactable = enabled; }