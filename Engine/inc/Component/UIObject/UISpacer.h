#pragma once

#include "Component/UIObject/UIObject.h"

class UISpacer : public UIObject
{
public:
	explicit UISpacer(float height = 10.0f);

	void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const override;

	float getSpacerHeight() const;
	void setSpacerHeight(float newHeight);

private:
	float spacerHeight;
};