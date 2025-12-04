#pragma once


#include "Component.h"
#include "Rendering/UIRenderCommand.h"

class IUserInterfaceRenderQueueWriter;

class UserInterfaceRenderComponent : virtual public Component
{
public:
	~UserInterfaceRenderComponent() = default;
	virtual void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const = 0;
};