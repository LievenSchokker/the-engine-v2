#pragma once

#include "Component/UIObject/UIObject.h"
#include "Rendering/Color.h"

class UIProgressBar : public UIObject
{
public:
	explicit UIProgressBar(
		float x = 0,
		float y = 0,
		float width = 150,
		float height = 20,
		float initialValue = 0.0f,
		float minValue = 0.0f,
		float maxValue = 1.0f
	);

	void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const override;

	float getValue() const;
	float getMinValue() const;
	float getMaxValue() const;
	float getNormalizedValue() const;

	void setValue(float newValue);
	void setMinValue(float newMin);
	void setMaxValue(float newMax);
	void setRange(float min, float max);

	Color getBarColor() const;
	Color getBackgroundColor() const;
	void setBarColor(const Color& color);
	void setBackgroundColor(const Color& color);

private:
	float value;
	float minValue;
	float maxValue;
	Color barColor = Color::green();
	Color backgroundColor = Color(50, 50, 50, 255);
};