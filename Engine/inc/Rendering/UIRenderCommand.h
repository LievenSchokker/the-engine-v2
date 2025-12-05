#pragma once

#include <string>
#include "Rendering/Color.h"
#include "Component/UIObject/Alignment.h"

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
};

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

	int getSortKey() const
	{
		return (layer << 8) | orderInLayer;
	}

	UIRenderCommand() = default;
};