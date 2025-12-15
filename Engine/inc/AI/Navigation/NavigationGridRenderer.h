//
// Created by samle on 14/12/2025.
//

#pragma once


#include "NavigationGrid.h"
#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"

struct GridRenderOptions
{
    float lineThickness = 1.0f;
    Color walkableCellsColor = Color::darkGreen();
    Color unwalkableCellsColor = Color::darkRed();
};


class NavigationGridRenderer : public Behaviour, public RenderComponent
{
    public:
        explicit NavigationGridRenderer() : grid(nullptr), gridRenderOptions(1.0f, Color::darkGreen(), Color::darkRed()){};

        void onAwake() override;

        /// Note: This should probably be passed via constructor once component args can be forwarded.
        void setGrid(NavigationGrid& grid);

        void fillRenderQueue(IRenderQueueWriter& queue) const override;

    private:
        NavigationGrid* grid;
        GridRenderOptions gridRenderOptions;

        std::vector<RenderCommand> cachedCommands;
        bool cacheBuilt = false;

    void buildCache();
};
