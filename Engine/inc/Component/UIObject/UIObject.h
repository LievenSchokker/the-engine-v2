#pragma once

#include "Component/BaseComponentTypes/UIRenderComponent.h"

class UIObject : public UserInterfaceRenderComponent
{
public:
	explicit UIObject(float x = 0, float y = 0, float width = 100, float height = 30);
	~UIObject() override = default;

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

	void setPanelId(uint32_t id);
	uint32_t getPanelId() const;

	void setParentId(uint32_t id);
	uint32_t getParentId() const;

protected:
	float x, y;
	float width, height;
	bool visible = true;
	uint32_t panelId = 0;
	uint32_t parentId = NO_PARENT;
};