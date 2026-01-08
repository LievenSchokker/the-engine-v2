#pragma once
#include "UIElement.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"

#include <string>

class UIButton final: public UIElement
{
public:
	explicit UIButton(
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

	void setPressedColor(const Color color) {  pressedColor = color;}
	void setHoverColor(const Color color) {  hoverColor = color;}
	void setNormalColor(const Color color) {  normalColor = color;}
	void setColor(const Color color)
	{
		setHoverColor(color);
		setPressedColor(color);
		setNormalColor(color);
	}

	Color getPressedColor() const { return pressedColor; }
	Color getHoverColor() const { return hoverColor; }
	Color getNormalColor() const { return normalColor; }
private:
	static uint32_t nextButtonId;

	uint32_t buttonId;
	std::string text;
	bool interactable = true;
	Color normalColor = Color(Color::gray());
	Color hoverColor = Color(Color::gray());
	Color pressedColor = Color(Color::gray());
	Color textColor = Color(Color::gray());
};