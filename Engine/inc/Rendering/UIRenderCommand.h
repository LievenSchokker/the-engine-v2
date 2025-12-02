// Rendering/UIRenderCommand.h
#pragma once

#include <string>
#include "Rendering/Color.h"
#include "Component/UIObject/Alignment.h"

constexpr uint32_t NO_PARENT = 0;

enum class UICommandType {
	Panel,
	Text,
	Button,
	Slider,
	Checkbox
};

struct UIRenderCommand {
	UICommandType type;
	uint32_t panelId;
	uint32_t parentId;

	float x = 0, y = 0, width = 100, height = 30;
	std::string title;
	bool hasTitle = false;
	bool hasBorder = false;
	float rowHeight = 30.0f;
	int columns = 1;

	// Text
	std::string text;
	Alignment alignment = Alignment::Left;
	Color color = Color::white();
	int fontSize = 14;
};