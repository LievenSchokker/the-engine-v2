#pragma once


#include "Rendering/Color.h"

#include <string>
#include <variant>

enum class TextAlignment {
	Left,
	Center,
	Right
};

struct TextRenderCommand {
	float x, y, width, height;
	std::string text;
	TextAlignment alignment = TextAlignment::Left;
	int fontSize = 14;
	Color color = Color::white();
};

struct ButtonRenderCommand {
	float x, y, width, height;
	std::string label;
	bool enabled = true;
	bool* wasClicked = nullptr;
};

using UIRenderCommand = std::variant<
	TextRenderCommand,
	ButtonRenderCommand
>;