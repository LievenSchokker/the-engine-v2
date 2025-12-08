#pragma once

#include "Component/UIObject/UIObject.h"

/**
 * @brief Invisible layout element that reserves vertical space between siblings.
 *
 * Provides explicit control over spacing without hardcoding gaps into other
 * elements. This keeps margins configurable per-layout rather than baked into
 * component defaults, and avoids the need for empty labels or padding hacks.
 */
class UISpacer: public UIObject
{
public:
	/// @param height Vertical space in pixels. Defaults to 10 for subtle
	///               separation; use larger values for section breaks.
	explicit UISpacer(float height = 10.0f);

	void fillUserInterfaceRenderQueue(
		IUserInterfaceRenderQueueWriter& queue) const override;

	float getSpacerHeight() const;
	void setSpacerHeight(float newHeight);

private:
	float spacerHeight;
};