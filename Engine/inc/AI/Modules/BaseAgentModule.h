//
// Created by samle on 02/12/2025.
//


#pragma once

class Agent;
class Transform;
struct Vector2;


class BaseAgentModule
{
    public:
        explicit BaseAgentModule(const Agent& _agent);
        virtual ~BaseAgentModule() = default;

        /**
         * @brief Method used to initialise this module.
         *
         * Gets called immediatly after adding the module to the agent, only once.
         * @return
         */
        virtual void initialise() {}

        /**
         * Computes the Vector needed to move the agent based on this module's logic.
         * @return the computed movement vector for this module.
         */
        virtual Vector2 compute() = 0;

    protected:
        const Agent& agent;
        const Transform& agentTransform;
};
