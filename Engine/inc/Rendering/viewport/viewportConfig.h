#pragma once


#include "Rendering/Rect.h"

struct ViewportConfig
{
	int logicalWidth = 1280;
	int logicalHeight = 720;

	int physicalWidth = 1280;
	int physicalHeight = 720;

	Rect letterboxRect;
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float pixelsPerMeter = 50.0f;
};