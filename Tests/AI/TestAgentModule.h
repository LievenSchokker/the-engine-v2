//
// Created by samle on 03/12/2025.
//

#pragma once


#include "AI/BaseAgentModule.h"
struct Vector2;

/**
 * This class serves as a testing module usefull in UnitTests.
 *
 * Compute method returns Vector2::one(), which represents a Vector2(1.0f, 1.0f).
 */
class TestAgentModule : public BaseAgentModule
{
    public:
        TestAgentModule() = default;
        ~TestAgentModule() override = default;

        /// Returns Vector2::one().
        Vector2 compute() override;
};
