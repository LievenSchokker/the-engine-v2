#include "Component/UIObject/Text.h"


Text::Text(const int width, const int height, const std::string& text,
		   const std::string& font, const int textSize,
		   const Alignment alignment)
	: UIObject(width, height),
	  text(text),
	  font(font),
	  textSize(textSize),
	  alignment(alignment)
{
}

std::string Text::getText() const
{
	return text;
}

std::string Text::getFont() const
{
	return font;
}

int Text::getTextSize() const
{
	return textSize;
}

Alignment Text::getAlignment() const
{
	return alignment;
}

void Text::setText(const std::string& newText)
{
	text = newText;
}

void Text::setFont(const std::string& newFont)
{
	font = newFont;
}

void Text::setTextSize(const int newTextSize)
{
	textSize = newTextSize;
}

void Text::setAlignment(const Alignment newAlignment)
{
	alignment = newAlignment;
}