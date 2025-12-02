#pragma once
#include "Component/UIObject/Alignment.h"
#include "Component/UIObject/UIObject.h"


class UITextObject: public UIObject
{
   public:
	explicit UITextObject(int width = 0, int height = 0, std::string  text = "",
		 std::string  font = "default", int textSize = 12,
		 Alignment alignment = Alignment::Left);

	std::string getText() const;
	std::string getFont() const;
	int getTextSize() const;
	Alignment getAlignment() const;

	void setText(const std::string& newText);
	void setFont(const std::string& newFont);
	void setTextSize(int newTextSize);
	void setAlignment(Alignment newAlignment);

   private:
	std::string text;
	std::string font;
	int textSize;
	Alignment alignment;
};
