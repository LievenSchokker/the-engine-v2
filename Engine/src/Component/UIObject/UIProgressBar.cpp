#include "Component/UIObject/UIProgressBar.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

#include <algorithm>

UIProgressBar::UIProgressBar(
    float x,
    float y,
    float width,
    float height,
    float initialValue,
    float minValue,
    float maxValue
)
    : UIObject(x, y, width, height)
    , value(initialValue)
    , minValue(minValue)
    , maxValue(maxValue)
{
}

void UIProgressBar::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
{
    if (!visible)
    {
        return;
    }

    UIRenderCommand command;
    command.type = UICommandType::ProgressBar;
    command.panelId = panelId;
    command.parentId = parentId;
    command.x = x;
    command.y = y;
    command.width = width;
    command.height = height;
    command.progress = getNormalizedValue();
    command.minValue = minValue;
    command.maxValue = maxValue;
    command.barColor = barColor;
    command.backgroundColor = backgroundColor;

    queue.push(command);
}

float UIProgressBar::getValue() const { return value; }
float UIProgressBar::getMinValue() const { return minValue; }
float UIProgressBar::getMaxValue() const { return maxValue; }

float UIProgressBar::getNormalizedValue() const
{
    if (maxValue <= minValue)
    {
        return 0.0f;
    }
    return std::clamp((value - minValue) / (maxValue - minValue), 0.0f, 1.0f);
}

void UIProgressBar::setValue(float newValue)
{
    value = std::clamp(newValue, minValue, maxValue);
}

void UIProgressBar::setMinValue(float newMin)
{
    minValue = newMin;
    value = std::clamp(value, minValue, maxValue);
}

void UIProgressBar::setMaxValue(float newMax)
{
    maxValue = newMax;
    value = std::clamp(value, minValue, maxValue);
}

void UIProgressBar::setRange(float min, float max)
{
    minValue = min;
    maxValue = max;
    value = std::clamp(value, minValue, maxValue);
}

Color UIProgressBar::getBarColor() const { return barColor; }
Color UIProgressBar::getBackgroundColor() const { return backgroundColor; }
void UIProgressBar::setBarColor(const Color& color) { barColor = color; }
void UIProgressBar::setBackgroundColor(const Color& color) { backgroundColor = color; }