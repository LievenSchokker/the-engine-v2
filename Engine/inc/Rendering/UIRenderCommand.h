#pragma once

#include <string>
#include "Rendering/Color.h"
#include "Component/UIElement/Alignment.h"

#include <functional>
#include <vector>

constexpr uint32_t NO_PARENT = 0;

enum class UICommandType
{
	Panel,
	Text,
	ProgressBar,
	Separator,
	Spacer,
	Image,
	Chart,
	Button,
};


/**
 * @brief A single UI draw instruction consumed by the render hook.
 *
 * Uses a flat struct rather than polymorphism so commands can be stored
 * contiguously, sorted cheaply, and processed without virtual dispatch.
 * Only a subset of fields is meaningful for each UICommandType—unused
 * fields are simply ignored during rendering.
 *
 * Field groups:
 *  - Panel:       x, y, width, height, title, hasTitle, hasBorder, rowHeight, columns
 *  - Text:        text, alignment, color, fontSize
 *  - ProgressBar: progress, minValue, maxValue, barColor, backgroundColor
 *  - Spacer:      spacerHeight
 *  - Image:       imagePath, textureId
 *  - Chart:       chartData, chartMin, chartMax, chartHeight
 */

struct UIRenderCommand
{
	UICommandType type = UICommandType::Panel;
	uint32_t panelId = NO_PARENT;
	uint32_t parentId = 0;

	float x = 0;
	float y = 0;
	float width = 100;
	float height = 30;

	std::string title{};
	bool hasTitle = false;
	bool hasBorder = false;
	float rowHeight = 30.0f;
	int columns = 1;

	std::string text{};
	Alignment alignment = Alignment::Left;
	Color color = Color::white();
	int fontSize = 14;

	float progress = 0.0f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	Color barColor = Color::green();
	Color backgroundColor = Color::black();

	float spacerHeight = 10.0f;

	std::string imagePath{};
	uint32_t textureId = 0;

	uint8_t layer = 0;
	uint8_t orderInLayer = 0;

	std::vector<float> chartData;
	float chartMin = 0.0f;
	float chartMax = 1.0f;
	int chartHeight = 50;

	uint32_t buttonId = 0;
	bool interactable = true;
	Color normalColor = Color(50, 50, 50, 255);
	Color hoverColor = Color(70, 70, 70, 255);
	Color pressedColor = Color(30, 30, 30, 255);
	Color textColor = Color::white();

	/**
	 * @brief Composite key for depth sorting.
	 *
	 * Packs layer and order into one integer so the render queue can
	 * sort with a single comparison rather than two.
	 */
	int getSortKey() const
	{
		return (layer << 8) | orderInLayer;
	}

	UIRenderCommand() = default;
};