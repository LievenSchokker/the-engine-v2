//
// Created by samle on 08/12/2025.
//


#pragma once


#include "Math/Vector2.h"

#include <vector>

/**
 * @brief Stores the result of a computed path by the NavigationSystem
 */
struct PathResult
{
    explicit PathResult(const std::vector<Vector2>& path_) : path(path_) {};

    /**
     * @brief Retrieves the computed path directly
     * @return a reference to the path, a std::vector of Vector2
     */
    const std::vector<Vector2>& getPath() const;

    /**
     * #@brief Returns the size of the path.
     * @return  the number of elements in the path
     */
    int getPathSize() const;

    /**
     * @brief Retrieves teh length of the path, calculated by measuring the distance between each point in the path
     * @return The length of the path, as float.
     */
    float getPathLength() const ;

    /**
     * @brief Checks whether the stored path is valid.
     * A path is valid if its size is > 0, and its length is > 0
     * @return true if the path is valid, false otherwise.
     */
    bool isValid() const;

    private:
        const std::vector<Vector2> path;
};
