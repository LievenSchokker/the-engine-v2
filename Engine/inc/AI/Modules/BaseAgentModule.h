//
// Created by samle on 02/12/2025.
//


#pragma once

class Agent;
class Transform;
struct Vector2;

/**
 * @brief This class serves as an abstract base class for all agent modules used by the SpelMotor engine
 *
 * Each concrete module is expected to implement the compute() method, which is used by @c Agent components to move the agent in a desired direction
 */
class BaseAgentModule
{
    public:
        /// Constructor takes in the agent that uses the module, allowing a module to gain access to methods on the component and the GameObject that stores the Agent component
        explicit BaseAgentModule(const Agent& _agent);
        virtual ~BaseAgentModule() = default;

        /**
         * @brief Method used to initialise this module.
         *
         * Gets called in the Agent's onAwake() method to ensure world references can be retrieved correctly.
         */
        virtual void initialise() {}

        /**
         * @brieff Computes a directional vector the agent should move in based on this module's definition.
         *
         * Interally, an Agent computes its velocity by summing all module's direction vector scaled by their weight.
         *
         */
        virtual Vector2 compute() = 0;

    protected:
        /// @brief Reference to the Agent component this module operates on
        const Agent& agent;

        /// @brief Reference to the Agent component's GameObject's Transform component
        const Transform& agentTransform;
};
