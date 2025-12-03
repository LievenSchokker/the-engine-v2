//
// Created by samle on 02/12/2025.
//


#pragma once


struct Vector2;


class BaseAgentModule
{
    public:
        BaseAgentModule() = default;
        virtual ~BaseAgentModule() = default;

        /**
         * Computes the Vector needed to move the agent based on this module's logic.
         * @return the computed movement vector for this module.
         */
        virtual Vector2 compute() = 0;
};
