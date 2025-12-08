#pragma once


#include "Component/BaseComponentTypes/UIRenderComponent.h"
#include "Rendering/UIRenderCommand.h"

/**
 * @brief Base class providing common layout and hierarchy for UI elements.
 *
 * Centralizes position, size, and visibility so derived classes can focus
 * on their specific rendering logic without reimplementing boilerplate.
 * The panel/parent ID system enables nested layouts—elements reference
 * their containing panel, allowing the render hook to group and position
 * children correctly without the UI objects needing to know about each other.
 */
class UIElement: public UserInterfaceRenderComponent
{
public:
	explicit UIElement(float x = 0, float y = 0, float width = 100,
	                  float height = 30);
	~UIElement() override = default;

	/// @name Geometry accessors
	/// @brief Screen-space coordinates. Origin is top-left; positive Y points down.
	/// @{
	float getX() const;
	float getY() const;
	float getWidth() const;
	float getHeight() const;

	void setX(float newX);
	void setY(float newY);
	void setWidth(float newWidth);
	void setHeight(float newHeight);
	/// @}

	/// @name Visibility
	/// @brief Hidden elements skip rendering entirely, avoiding unnecessary
	///        draw calls rather than drawing transparent quads.
	/// @{
	bool isVisible() const;
	void setVisible(bool newVisible);
	/// @}

	/// @name Panel hierarchy
	/// @brief panelId identifies this element; parentId links it to a
	///        containing panel. NO_PARENT marks root-level elements.
	///        This flat ID scheme avoids pointer ownership issues when
	///        commands are copied into the render queue.
	/// @{
	void setPanelId(uint32_t id);
	uint32_t getPanelId() const;

	void setParentId(uint32_t id);
	uint32_t getParentId() const;
	/// @}

protected:
	float x, y;
	float width, height;
	bool visible = true;
	uint32_t panelId = 0;
	uint32_t parentId = NO_PARENT;
};