#pragma once
#include "Component/UIObject/Alignment.h"
#include "Component/UIObject/UIObject.h"


class Text: public UIObject
{
   public:
	explicit Text(int width = 0, int height = 0, const std::string& text = "",
		 const std::string& font = "Arial", int textSize = 12,
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
	// QQToDo: Color depends on rendering
};
