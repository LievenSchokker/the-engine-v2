#pragma once

#include "Component/UIObject/UIObject.h"
#include "Component/UIObject/Alignment.h"
#include "Rendering/Color.h"

#include <string>

class UITextObject: public UIObject
{
public:
	explicit UITextObject(
		float x = 0,
		float y = 0,
		float width = 100,
		float height = 30,
		std::string text = "",
		int fontSize = 14,
		Alignment alignment = Alignment::Left,
		Color color = Color::white()
		);

	void fillUserInterfaceRenderQueue(
		IUserInterfaceRenderQueueWriter& queue) const override;

	std::string getText() const;
	int getFontSize() const;
	Alignment getAlignment() const;
	Color getColor() const;

	void setText(const std::string& newText);
	void setFontSize(int newSize);
	void setAlignment(Alignment newAlignment);
	void setColor(const Color& newColor);

private:
	std::string text;
	int fontSize;
	Alignment alignment;
	Color color;
};