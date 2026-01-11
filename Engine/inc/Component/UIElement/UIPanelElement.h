#pragma once

#include "Behaviour/Behaviour.h"
#include "Component/UIElement/UIElement.h"
#include "Docking.h"
#include "Math/Vector2.h"

#include <string>

/**
 * @brief Container element that groups and layouts child UI objects.
 *
 * Panels serve as the structural backbone of the UI hierarchy. Child elements
 * reference a panel's ID via their parentId, and the render hook uses the
 * panel's layout settings to arrange them automatically. This decouples
 * positioning logic from individual elements—a label doesn't need to know
 * its screen coordinates, just which panel it belongs to.
 *
 * Row height and column count define a simple grid layout. The render hook
 * flows children left-to-right, top-to-bottom within that grid, avoiding
 * the need for manual coordinate math in game code.
 */
class UIPanelElement: public UIElement
{
public:

	explicit UIPanelElement(
		float x = 0,
		float y = 0,
		float width = 200,
		float height = 200,
		std::string title = ""
		);

	void fillUserInterfaceRenderQueue(
		IUserInterfaceRenderQueueWriter& queue) const override;

	std::string getTitle() const;
	bool hasBorder() const;
	bool hasTitle() const;
	float getRowHeight() const;
	int getColumns() const;

	void setTitle(const std::string& newTitle);
	void setBorder(bool enabled);
	void setBackgroundColor(Color color);
	Color getBorderColor() const;
	float getBorderThickness() const;
	float getPadding() const;
	float getSpacing() const;
	void setRowHeight(float height);
	void setColumns(int cols);

	void setBorderColor(Color color);
	void setBorderThickness(float thickness);
	void setPadding(float pad);
	void setSpacing(float space);

	Color getTitleBackgroundColor() const;
	Color getTitleTextColor() const;
	void setTitleBackgroundColor(Color color);
	void setTitleTextColor(Color color);

	bool isScrollable() const;
	bool isResizable() const;
	bool isMinimizable() const;
	bool isClosable() const;

	void setScrollable(bool enabled);
	void setResizable(bool enabled);
	void setMinimizable(bool enabled);
	void setClosable(bool enabled);

	float getTitleHeight() const;
	float getTitlePadding() const;
	float getTitleLabelPadding() const;

	void setTitleHeight(float height);
	void setTitlePadding(float pad);
	void setTitleLabelPadding(float pad);

	void setDock(UIDock dock);
	UIDock getDock() const;
private:
	UIDock dock = UIDock::None;
	std::string title;

	float borderThickness = 1.0f;
	float padding = 4.0f;
	float spacing = 4.0f;
	float opacity = 1.0f;

	Color titleBackgroundColor = Color(40, 40, 40, 255);
	Color titleTextColor = Color::white();
	Color borderColor = Color::white();

	Alignment titleAlignment = Alignment::Left;

	bool scrollable = false;
	bool horizontalScrollbar = false;
	bool verticalScrollbar = false;
	Vector2 scrollOffset = {0, 0};

	float titleHeight = 24.0f;
	float titlePadding = 4.0f;
	float titleLabelPadding = 4.0f;

	bool resizable = false;
	bool minimizable = false;
	bool closable = false;
	bool isMinimized = false;
	/// Visual border around the panel edges. Disabled by default to allow
	/// seamless embedding; enable for distinct visual grouping.
	bool border = false;

	/// Uniform height for each row in the grid layout. Keeps text and
	/// controls vertically aligned without per-element tweaking.
	float rowHeight = 30.0f;

	/// Number of elements per row before wrapping. Single-column (default)
	/// produces a vertical list; increase for toolbars or button grids.
	int columns = 1;

	Color backgroundColor = Color::blue();
};