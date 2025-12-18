//
// Created by samle on 14/12/2025.
//

#pragma once
#include "../IZandbak.h"

class PathFindingZandbak : public IZandbak
{
    public:
        PathFindingZandbak() = default;
        ~PathFindingZandbak() override = default;

        std::unique_ptr<Scene> getScene() override;

    private:
        static std::vector<std::unique_ptr<GameObject>> createObstacles();
};
