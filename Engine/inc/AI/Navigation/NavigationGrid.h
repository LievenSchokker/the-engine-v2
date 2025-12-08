//
// Created by samle on 08/12/2025.
//

#pragma once

#include "Component/BaseComponentTypes/Component.h"
class NavigationCell;
struct Vector2;


class NavigationGrid : public Component
{
    public:
        NavigationGrid() = default;
        ~NavigationGrid() override = default;

        void initialise(Vector2 dimensions);
        bool isWalkable(Vector2 cellPosition) const;
        void setWalkable(Vector2 cellPosition, bool value);
        float getCellWeight(Vector2 cellPosition);
        void setCellWeight(Vector2 cellPosition, float weight);

        bool isValidCell(Vector2 cellPosition) const;

    private:
        int width;
        int height;
        std::vector<NavigationCell> cells;
};
