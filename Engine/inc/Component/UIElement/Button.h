#pragma once
#include "UIElement.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"

#include <string>

class Button : public UIElement
{
public:
	explicit Button(
		float x = 0,
		float y = 0,
		float width = 100,
		float height = 30,
		std::string text = "Button"
	);

	void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const override;

	std::string getText() const;
	bool isInteractable() const;
	uint32_t getButtonId() const;

	void setText(const std::string& newText);
	void setInteractable(bool enabled);

private:
	static uint32_t nextButtonId;

	uint32_t buttonId;
	std::string text;
	bool interactable = true;
};