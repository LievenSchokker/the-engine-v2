//
// Created by samle on 14/12/2025.
//

#pragma once
#include "NavigationGrid.h"
#include "Component/BaseComponentTypes/RenderComponent.h"

class NavigationGridRenderer : public RenderComponent
{
    public:
        explicit NavigationGridRenderer();

        /// Note: This should be passed via constructor once component args can be forwarded.
        void setGrid(NavigationGrid& grid);

        void fillRenderQueue(IRenderQueueWriter& queue) const override;

    private:
        NavigationGrid* grid;
};
