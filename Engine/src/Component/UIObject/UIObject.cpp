#include "Component/UIObject/UIObject.h"


UIObject::UIObject(const int width, const int height)
	: width(width), height(height)
{
}

UIObject::~UIObject() = default;

int UIObject::getWidth() const
{
	return width;
}

int UIObject::getHeight() const
{
	return height;
}

void UIObject::setWidth(const int newWidth)
{
	width = newWidth;
}

void UIObject::setHeight(const int newHeight)
{
	height = newHeight;
}