#include "Component/UIObject/UIObject.h"

UIObject::UIObject(float x, float y, float width, float height)
	: x(x), y(y), width(width), height(height) {
}

UIObject::~UIObject() = default;

float UIObject::getX() const {
	return x;
}

float UIObject::getY() const {
	return y;
}

float UIObject::getWidth() const {
	return width;
}

float UIObject::getHeight() const {
	return height;
}

void UIObject::setX(float newX) {
	x = newX;
}

void UIObject::setY(float newY) {
	y = newY;
}

void UIObject::setWidth(float newWidth) {
	width = newWidth;
}

void UIObject::setHeight(float newHeight) {
	height = newHeight;
}

bool UIObject::isVisible() const {
	return visible;
}

void UIObject::setVisible(bool newVisible) {
	visible = newVisible;
}