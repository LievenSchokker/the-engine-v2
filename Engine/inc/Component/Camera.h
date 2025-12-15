#pragma once
#include "BaseComponentTypes/Component.h"

class Camera final: public Component
{
public:
	Camera(const float offsetX_, const float offsetY_, const uint32_t width_,
	       const uint32_t height_)
		: width(width_),
		  height(height_),
		  offsetX(offsetX_),
		  offsetY(offsetY_)
	{
	}

	float getOffsetX() const
	{
		return offsetX;
	}

	float getOffsetY() const
	{
		return offsetY;
	}

	~Camera() override = default;

private:
	uint32_t width = 500;
	uint32_t height = 500;

	float offsetX = 0;
	float offsetY = 0;
};