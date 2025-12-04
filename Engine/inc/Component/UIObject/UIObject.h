#pragma once


#include "Component/BaseComponentTypes/Component.h"

class UIObject: public Component
{
public:
	explicit UIObject(float x = 0, float y = 0, float width = 0, float height = 0);
	~UIObject() override;

	float getX() const;
	float getY() const;
	float getWidth() const;
	float getHeight() const;

	void setX(float newX);
	void setY(float newY);
	void setWidth(float newWidth);
	void setHeight(float newHeight);

	bool isVisible() const;
	void setVisible(bool newVisible);

private:
	float x, y;
	float width, height;
	bool visible = true;
};
