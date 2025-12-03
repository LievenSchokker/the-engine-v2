#pragma once
#include "../BaseComponentTypes/Component.h"


class UIObject: public Component
{
   public:
	explicit UIObject(int width = 0, int height = 0);
	~UIObject() override;

	int getWidth() const;
	int getHeight() const;

	void setWidth(const int newWidth);
	void setHeight(const int newHeight);

   private:
	int width;
	int height;
};
