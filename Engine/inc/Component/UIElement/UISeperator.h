#pragma once

#include "Component/UIElement/UIElement.h"

class UISeparator : public UIElement
{
public:
	explicit UISeparator();

	void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const override;
};