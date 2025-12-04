#include "Component/UIObject/Button.h"
#include "Component/UIObject/UITextObject.h"
#include "Component/UIObject/UIObject.h"

#include <gtest/gtest.h>

namespace engine_tests
{
TEST(UIObjectTests, DefaultUIObject)
{
	UIObject uiObject;

	EXPECT_EQ(uiObject.getWidth(), 0);
	EXPECT_EQ(uiObject.getHeight(), 0);
}

TEST(UIObjectTests, CustomUIObject)
{
	UIObject uiObject(10, 20, 10, 20);

	EXPECT_EQ(uiObject.getWidth(), 10);
	EXPECT_EQ(uiObject.getHeight(), 20);
}

TEST(UIObjectTests, SetWidth)
{
	UIObject uiObject;
	uiObject.setWidth(10);

	EXPECT_EQ(uiObject.getWidth(), 10);
}

TEST(UIObjectTests, SetHeight)
{
	UIObject uiObject;
	uiObject.setHeight(10);

	EXPECT_EQ(uiObject.getHeight(), 10);
}

TEST(ButtonTests, DefaultButton)
{
	Button button;

	EXPECT_EQ(button.getWidth(), 0);
	EXPECT_EQ(button.getHeight(), 0);
	EXPECT_FALSE(button.getInteractable());
}

TEST(ButtonTests, CustomButton)
{
	Button button(20, 20, true);

	EXPECT_EQ(button.getWidth(), 20);
	EXPECT_EQ(button.getHeight(), 20);
	EXPECT_TRUE(button.getInteractable());
}

TEST(ButtonTests, SetInteractable)
{
	Button button(20, 20, true);

	button.setInteractable(false);

	EXPECT_FALSE(button.getInteractable());
}

TEST(TextTests, DefaultTextObject)
{
	UITextObject text;

	EXPECT_EQ(text.getWidth(), 0);
	EXPECT_EQ(text.getHeight(), 0);
	EXPECT_EQ(text.getText(), "");
	EXPECT_EQ(text.getFont(), "default");
	EXPECT_EQ(text.getTextSize(), 12);
	EXPECT_EQ(text.getAlignment(), Alignment::Left);
}

TEST(TextTests, CustomTextObject)
{
	UITextObject text(10, 10, "Hello World", "Arial2", 10, Alignment::Right);

	EXPECT_EQ(text.getText(), "Hello World");
	EXPECT_EQ(text.getFont(), "Arial2");
	EXPECT_EQ(text.getTextSize(), 10);
	EXPECT_EQ(text.getAlignment(), Alignment::Right);
}

TEST(TextTests, SetText)
{
	UITextObject text(10, 10, "Hello World");

	text.setText("Goodbye World");

	EXPECT_EQ(text.getText(), "Goodbye World");
}

TEST(TextTests, SetFont)
{
	UITextObject text(10, 10, "Hello World", "NotArial");

	text.setFont("Arial");

	EXPECT_EQ(text.getFont(), "Arial");
}

TEST(TextTests, SetTextSize)
{
	UITextObject text(10, 10, "Hello World", "Arial", 10, Alignment::Right);

	text.setTextSize(20);

	EXPECT_EQ(text.getTextSize(), 20);
}

TEST(TextTests, SetAlignment)
{
	UITextObject text(10, 10, "Hello World", "Arial", 10, Alignment::Left);

	text.setAlignment(Alignment::Left);

	EXPECT_EQ(text.getAlignment(), Alignment::Left);
}
}