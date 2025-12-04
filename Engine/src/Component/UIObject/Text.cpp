#include <utility>

#include "Component/UIObject/UITextObject.h"


UITextObject::UITextObject(const int width, const int height, std::string  text,
		   std::string  font, const int textSize,
		   const Alignment alignment)
	: UIObject(width, height),
	  text(std::move(text)),
	  font(std::move(font)),
	  textSize(textSize),
	  alignment(alignment)
{
}

std::string UITextObject::getText() const
{
	return text;
}

std::string UITextObject::getFont() const
{
	return font;
}

int UITextObject::getTextSize() const
{
	return textSize;
}

Alignment UITextObject::getAlignment() const
{
	return alignment;
}

void UITextObject::setText(const std::string& newText)
{
	text = newText;
}

void UITextObject::setFont(const std::string& newFont)
{
	font = newFont;
}

void UITextObject::setTextSize(const int newTextSize)
{
	textSize = newTextSize;
}

void UITextObject::setAlignment(const Alignment newAlignment)
{
	alignment = newAlignment;
}