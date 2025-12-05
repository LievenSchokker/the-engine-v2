#pragma once

#include "Component/UIObject/UIObject.h"

class UISeparator : public UIObject
{
public:
	explicit UISeparator();

	void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const override;
};