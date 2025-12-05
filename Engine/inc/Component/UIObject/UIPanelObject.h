#pragma once


#include "Component/UIObject/UIObject.h"


#include <string>

class UIPanelObject : public UIObject
{
public:
	explicit UIPanelObject(
		float x = 0,
		float y = 0,
		float width = 200,
		float height = 200,
		std::string title = ""
	);

	void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const override;

	std::string getTitle() const;
	bool hasBorder() const;
	bool hasTitle() const;
	float getRowHeight() const;
	int getColumns() const;

	void setTitle(const std::string& newTitle);
	void setBorder(bool enabled);
	void setRowHeight(float height);
	void setColumns(int cols);

private:
	std::string title;
	bool border = false;
	float rowHeight = 30.0f;
	int columns = 1;
};