#pragma once

#include "Component/UIObject/UIObject.h"
#include "Component/UIObject/Alignment.h"
#include "Rendering/Color.h"

#include <string>

/**
 * @brief Single-line text display for labels, values, and headings.
 *
 * Wraps text rendering with panel-aware positioning so callers don't need
 * to manually calculate screen coordinates. Alignment is relative to the
 * element's bounds within its parent panel, making it easy to right-align
 * numeric values or center titles without pixel math.
 */
class UITextObject: public UIObject
{
public:
	explicit UITextObject(
		float x = 0,
		float y = 0,
		float width = 100,
		float height = 30,
		std::string text = "",
		int fontSize = 14,
		Alignment alignment = Alignment::Left,
		Color color = Color::white()
		);

	void fillUserInterfaceRenderQueue(
		IUserInterfaceRenderQueueWriter& queue) const override;


	std::string getText() const;
	int getFontSize() const;
	Alignment getAlignment() const;
	Color getColor() const;

	void setText(const std::string& newText);
	void setFontSize(int newSize);
	void setAlignment(Alignment newAlignment);
	void setColor(const Color& newColor);

private:
	std::string text;

	/// Font size in points. Stored here rather than relying on a global
	/// default so different text elements can coexist at varying scales
	/// (e.g., headings vs. body text).
	int fontSize;

	/// Horizontal alignment within the element's bounding box. Vertical
	/// alignment isn't exposed because single-line text is typically
	/// centered vertically by the render hook.
	Alignment alignment;

	Color color;
};