#include "Rendering/Color.h"

Color Color::fromRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	return Color(red, green, blue, alpha);
}

Color Color::black(uint8_t alpha)
{
	return Color(0, 0, 0, alpha);
}

Color Color::white(uint8_t alpha)
{
	return Color(255, 255, 255, alpha);
}

Color Color::blue(uint8_t alpha)
{
	return Color(0, 122, 255, alpha);
}

Color Color::yellow(uint8_t alpha)
{
	return Color(255, 221, 0, alpha);
}

Color Color::red(uint8_t alpha)
{
	return Color(255, 0, 0, alpha);
}

Color Color::green(uint8_t alpha)
{
	return Color(0, 255, 0, alpha);
}

Color Color::purple(uint8_t alpha)
{
	return Color(128, 0, 128, alpha);
}

Color Color::orange(uint8_t alpha)
{
	return Color(255, 165, 0, alpha);
}

Color Color::pink(uint8_t alpha)
{
	return Color(255, 192, 203, alpha);
}

Color Color::brown(uint8_t alpha)
{
	return Color(165, 42, 42, alpha);
}

Color Color::gray(uint8_t alpha)
{
	return Color(128, 128, 128, alpha);
}

Color Color::lightGray(uint8_t alpha)
{
	return Color(211, 211, 211, alpha);
}

Color Color::darkGray(uint8_t alpha)
{
	return Color(169, 169, 169, alpha);
}

Color Color::lightBlue(uint8_t alpha)
{
	return Color(173, 216, 230, alpha);
}

Color Color::darkBlue(uint8_t alpha)
{
	return Color(0, 0, 139, alpha);
}

Color Color::lightGreen(uint8_t alpha)
{
	return Color(144, 238, 144, alpha);
}

Color Color::darkGreen(uint8_t alpha)
{
	return Color(0, 100, 0, alpha);
}

Color Color::lightRed(uint8_t alpha)
{
	return Color(255, 102, 102, alpha);
}

Color Color::darkRed(uint8_t alpha)
{
	return Color(139, 0, 0, alpha);
}

Color Color::lightYellow(uint8_t alpha)
{
	return Color(255, 255, 153, alpha);
}

Color Color::darkYellow(uint8_t alpha)
{
	return Color(204, 204, 0, alpha);
}

Color Color::lightPurple(uint8_t alpha)
{
	return Color(221, 160, 221, alpha);
}

Color Color::darkPurple(uint8_t alpha)
{
	return Color(75, 0, 130, alpha);
}

Color Color::lightOrange(uint8_t alpha)
{
	return Color(255, 200, 124, alpha);
}

Color Color::darkOrange(uint8_t alpha)
{
	return Color(255, 140, 0, alpha);
}

Color Color::lightPink(uint8_t alpha)
{
	return Color(255, 182, 193, alpha);
}