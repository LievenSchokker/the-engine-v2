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

	static Color black(uint8_t alpha = 255);

	static Color white(uint8_t alpha = 255);

	static Color blue(uint8_t alpha = 255);

	static Color yellow(uint8_t alpha = 255);

	static Color red(uint8_t alpha = 255);

	static Color green(uint8_t alpha = 255);

	static Color purple(uint8_t alpha = 255);

	static Color orange(uint8_t alpha = 255);

	static Color pink(uint8_t alpha = 255);

	static Color brown(uint8_t alpha = 255);

	static Color gray(uint8_t alpha = 255);

	static Color lightGray(uint8_t alpha = 255);

	static Color darkGray(uint8_t alpha = 255);

	static Color lightBlue(uint8_t alpha = 255);

	static Color darkBlue(uint8_t alpha = 255);

	static Color lightGreen(uint8_t alpha = 255);

	static Color darkGreen(uint8_t alpha = 255);

	static Color lightRed(uint8_t alpha = 255);

	static Color darkRed(uint8_t alpha = 255);

	static Color lightYellow(uint8_t alpha = 255);

	static Color darkYellow(uint8_t alpha = 255);

	static Color lightPurple(uint8_t alpha = 255);

	static Color darkPurple(uint8_t alpha = 255);

	static Color lightOrange(uint8_t alpha = 255);

	static Color darkOrange(uint8_t alpha = 255);

	static Color lightPink(uint8_t alpha = 255);
};
