#pragma once

#include <cstdint>

/**
 * @brief RGBA color stored as 8-bit channels.
 */
struct Color {
	constexpr Color() = default;

	constexpr Color(uint8_t red, uint8_t green, uint8_t blue,
					uint8_t alpha = 255)
		: r(red), g(green), b(blue), a(alpha)
	{
	}

	uint8_t r = 255;
	uint8_t g = 255;
	uint8_t b = 255;
	uint8_t a = 255;

	bool operator==(const Color& other) const
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}

	static Color fromRGBA(uint8_t red, uint8_t green, uint8_t blue,
						  uint8_t alpha = 255);

	static Color Black(uint8_t alpha = 255);

	static Color White(uint8_t alpha = 255);

	static Color Blue(uint8_t alpha = 255);

	static Color Yellow(uint8_t alpha = 255);

	static Color Red(uint8_t alpha = 255);

	static Color Green(uint8_t alpha = 255);

	static Color Purple(uint8_t alpha = 255);

	static Color Orange(uint8_t alpha = 255);

	static Color Pink(uint8_t alpha = 255);

	static Color Brown(uint8_t alpha = 255);

	static Color Gray(uint8_t alpha = 255);

	static Color LightGray(uint8_t alpha = 255);

	static Color DarkGray(uint8_t alpha = 255);

	static Color LightBlue(uint8_t alpha = 255);

	static Color DarkBlue(uint8_t alpha = 255);

	static Color LightGreen(uint8_t alpha = 255);

	static Color DarkGreen(uint8_t alpha = 255);

	static Color LightRed(uint8_t alpha = 255);

	static Color DarkRed(uint8_t alpha = 255);

	static Color LightYellow(uint8_t alpha = 255);

	static Color DarkYellow(uint8_t alpha = 255);

	static Color LightPurple(uint8_t alpha = 255);

	static Color DarkPurple(uint8_t alpha = 255);

	static Color LightOrange(uint8_t alpha = 255);

	static Color DarkOrange(uint8_t alpha = 255);

	static Color LightPink(uint8_t alpha = 255);
};
