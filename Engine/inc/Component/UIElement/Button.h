#pragma once


#include "Component/UIElement/UIElement.h"

class Button: public UIElement
{
public:
	explicit Button(int width = 0, int height = 0, bool interactable = false);

	void setInteractable(bool newInteractable);
	bool getInteractable() const;

	void onClick();

private:
	bool interactable;
};