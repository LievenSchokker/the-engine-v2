#include "Component/UIElement/UIElement.h"
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

	/// @name Panel properties
	/// @{
	std::string getTitle() const;
	bool hasBorder() const;
	bool hasTitle() const;
	float getRowHeight() const;
	int getColumns() const;

	void setTitle(const std::string& newTitle);
	void setBorder(bool enabled);
	void setRowHeight(float height);
	void setColumns(int cols);
	/// @}

private:
	std::string title;

	/// Visual border around the panel edges. Disabled by default to allow
	/// seamless embedding; enable for distinct visual grouping.
	bool border = false;

	/// Uniform height for each row in the grid layout. Keeps text and
	/// controls vertically aligned without per-element tweaking.
	float rowHeight = 30.0f;

	/// Number of elements per row before wrapping. Single-column (default)
	/// produces a vertical list; increase for toolbars or button grids.
	int columns = 1;
};