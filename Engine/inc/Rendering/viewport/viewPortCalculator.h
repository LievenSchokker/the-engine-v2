#pragma once
#include "Math/Vector2.h"
#include "viewportConfig.h"

class ViewportCalculator
{
public:
	static void recalculate(ViewportConfig& config)
	{
		float targetAspect = static_cast<float>(config.logicalWidth) / config.logicalHeight;
		float windowAspect = static_cast<float>(config.physicalWidth) / config.physicalHeight;

		if (windowAspect > targetAspect)
		{
			config.letterboxRect.h = config.physicalHeight;
			config.letterboxRect.w = static_cast<int>(config.physicalHeight * targetAspect);
			config.letterboxRect.x = (config.physicalWidth - config.letterboxRect.w) / 2;
			config.letterboxRect.y = 0;
		}
		else
		{
			config.letterboxRect.w = config.physicalWidth;
			config.letterboxRect.h = static_cast<int>(config.physicalWidth / targetAspect);
			config.letterboxRect.x = 0;
			config.letterboxRect.y = (config.physicalHeight - config.letterboxRect.h) / 2;
		}

		config.scaleX = static_cast<float>(config.logicalWidth) / config.letterboxRect.w;
		config.scaleY = static_cast<float>(config.logicalHeight) / config.letterboxRect.h;
	}

	static Vector2 physicalToLogical(const ViewportConfig& config, int physX, int physY)
	{
		return {
			(physX - config.letterboxRect.x) * config.scaleX,
			(physY - config.letterboxRect.y) * config.scaleY
		};
	}
};