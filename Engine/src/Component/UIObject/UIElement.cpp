#include "Component/UIElement/UIElement.h"

UIElement::UIElement(float x, float y, float width, float height)
	: x(x)
	, y(y)
	, width(width)
	, height(height)
{
}

float UIElement::getX() const { return x; }
float UIElement::getY() const { return y; }
float UIElement::getWidth() const { return width; }
float UIElement::getHeight() const { return height; }

void UIElement::setX(float newX) { x = newX; }
void UIElement::setY(float newY) { y = newY; }
void UIElement::setWidth(float newWidth) { width = newWidth; }
void UIElement::setHeight(float newHeight) { height = newHeight; }

bool UIElement::isVisible() const { return visible; }
void UIElement::setVisible(bool newVisible) { visible = newVisible; }

void UIElement::setPanelId(uint32_t id) { panelId = id; }
uint32_t UIElement::getPanelId() const { return panelId; }

void UIElement::setParentId(uint32_t id) { parentId = id; }
uint32_t UIElement::getParentId() const { return parentId; }