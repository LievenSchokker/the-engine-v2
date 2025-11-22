#pragma once
#include "Component/UIObject/UIObject.h"


class Button: public UIObject
{
   public:
	explicit Button(int width = 0, int height = 0, bool interactable = false);

	void setInteractable(bool newInteractable);
	bool getInteractable() const;

	void onClick();

   private:
	bool interactable;
};