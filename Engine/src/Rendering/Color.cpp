#include "../../inc/Rendering/Color.h"

Color Color::fromRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	return Color(red, green, blue, alpha);
}

Color Color::Black(uint8_t alpha)
{
	return Color(0, 0, 0, alpha);
}

Color Color::White(uint8_t alpha)
{
	return Color(255, 255, 255, alpha);
}

Color Color::Blue(uint8_t alpha)
{
	return Color(0, 122, 255, alpha);
}

Color Color::Yellow(uint8_t alpha)
{
	return Color(255, 221, 0, alpha);
}

Color Color::Red(uint8_t alpha)
{
	return Color(255, 0, 0, alpha);
}

Color Color::Green(uint8_t alpha)
{
	return Color(0, 255, 0, alpha);
}

Color Color::Purple(uint8_t alpha)
{
	return Color(128, 0, 128, alpha);
}

Color Color::Orange(uint8_t alpha)
{
	return Color(255, 165, 0, alpha);
}

Color Color::Pink(uint8_t alpha)
{
	return Color(255, 192, 203, alpha);
}

Color Color::Brown(uint8_t alpha)
{
	return Color(165, 42, 42, alpha);
}

Color Color::Gray(uint8_t alpha)
{
	return Color(128, 128, 128, alpha);
}

Color Color::LightGray(uint8_t alpha)
{
	return Color(211, 211, 211, alpha);
}

Color Color::DarkGray(uint8_t alpha)
{
	return Color(169, 169, 169, alpha);
}

Color Color::LightBlue(uint8_t alpha)
{
	return Color(173, 216, 230, alpha);
}

Color Color::DarkBlue(uint8_t alpha)
{
	return Color(0, 0, 139, alpha);
}

Color Color::LightGreen(uint8_t alpha)
{
	return Color(144, 238, 144, alpha);
}

Color Color::DarkGreen(uint8_t alpha)
{
	return Color(0, 100, 0, alpha);
}

Color Color::LightRed(uint8_t alpha)
{
	return Color(255, 102, 102, alpha);
}

Color Color::DarkRed(uint8_t alpha)
{
	return Color(139, 0, 0, alpha);
}

Color Color::LightYellow(uint8_t alpha)
{
	return Color(255, 255, 153, alpha);
}

Color Color::DarkYellow(uint8_t alpha)
{
	return Color(204, 204, 0, alpha);
}

Color Color::LightPurple(uint8_t alpha)
{
	return Color(221, 160, 221, alpha);
}

Color Color::DarkPurple(uint8_t alpha)
{
	return Color(75, 0, 130, alpha);
}

Color Color::LightOrange(uint8_t alpha)
{
	return Color(255, 200, 124, alpha);
}

Color Color::DarkOrange(uint8_t alpha)
{
	return Color(255, 140, 0, alpha);
}

Color Color::LightPink(uint8_t alpha)
{
	return Color(255, 182, 193, alpha);
}