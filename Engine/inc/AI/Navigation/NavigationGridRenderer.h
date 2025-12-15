//
// Created by samle on 14/12/2025.
//

#pragma once
#include "NavigationGrid.h"
#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"

class NavigationGridRenderer : public Behaviour, public RenderComponent
{
    public:
        explicit NavigationGridRenderer() : grid(nullptr) {};

        void onAwake() override;

        /// Note: This should be passed via constructor once component args can be forwarded.
        void setGrid(NavigationGrid& grid);

        void fillRenderQueue(IRenderQueueWriter& queue) const override;

    private:
        NavigationGrid* grid;

    std::vector<RenderCommand> cachedCommands;
    bool cacheBuilt = false;

    void buildCache();
};
