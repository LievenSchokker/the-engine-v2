#pragma once


#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "NavigationGrid.h"


/// @brief Rendering options belonging to the @c NavigationGridRenderer
struct GridRenderOptions
{
	float lineThickness = 1.0f;
	Color walkableCellsColor = Color::white();
	Color unwalkableCellsColor = Color::black();
};


/**
 * @brief RenderComponent that visualises an @c NavigationGrid by drawing wireframe rectangles around each of its cells.
 */
class NavigationGridRenderer: public Behaviour, public RenderComponent
{
public:
	explicit NavigationGridRenderer() : grid(nullptr),
										gridRenderOptions{
											1.0f, Color::darkGreen(),
											Color::darkRed()}
	{
	};

	explicit
	NavigationGridRenderer(GridRenderOptions renderOptions) : grid(nullptr),
		gridRenderOptions(renderOptions)
	{
	};

	/**
	 * @brief onAwake is used to attempt to retrieve the scene's NavigationGrid.
	 * If it does not succeed, this behaviour gets silently disabled to prevent errors occuring.
	 */
	void onAwake() override;

	/**
	 * @brief Manually set a reference to the grid
	 * @param grid
	 */
	void setGrid(NavigationGrid& grid);

	/**
	 * @brief Draws wireframe style rectangles around each grid cell, using the @c gridRenderOptions
	 * @param queue
	 */
	void fillRenderQueue(IRenderQueueWriter& queue) const override;

private:
	/// We cache the renderCommands because otherwise performance == ATROCIOUS
	void buildCache();

	std::vector<RenderCommand> cachedCommands;
	bool cacheBuilt = false;

	NavigationGrid* grid;
	GridRenderOptions gridRenderOptions;
};