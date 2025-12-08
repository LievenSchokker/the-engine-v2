#include "Component/UIElement/Button.h"

#include <iostream>


Button::Button(const int width, const int height, const bool interactable)
	: UIElement(0, 0, width, height), interactable(interactable)
{
}

void Button::setInteractable(const bool newInteractable)
{
	interactable = newInteractable;
}

bool Button::getInteractable() const
{
	return interactable;
}

void Button::onClick()
{
	//  QQToDo: Implementation of this function depends on the GUI choice
	std::cout << "Button has been clicked!" << std::endl;
}
