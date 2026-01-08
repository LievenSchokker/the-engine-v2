//
// Created by samle on 02/12/2025.
//


#pragma once


#include "Behaviour/Behaviour.h"
#include "Networking/Serialization/RegistrationBase.h"
#include "Math/Vector2.h"
#include "Navigation/Pathfinding/AStarPathFinder.h"

struct ModuleData;
enum class ModuleStatus;
enum class ModuleType;
class RigidBody;


/**
 * @brief Agent component is a behaviour that can be used to have autonomically moving GameObjects.
 *
 * An Agent uses @c BaseAgentModules to move around the area. Modules can have weights to impact their influence on the Agent's behaviour.
 * Each update cycle, the Agent computes a direction vector based on the modules, scales them with their weight, resulting in a single desired force vector.
 * This vector is then used to set the velocity of the agent, clamping it to a maximum.
 */
class Agent final : public Behaviour, public RegistrationBase<Agent>
{
    public:
        Agent()
            : currentVelocity(Vector2::zero()),
              maxSpeed(1),
              arrivingDistance(10.0f),
              rotationTurnRate(90),
              accelerationMultiplier(12.0f),
              minAcceleration(120.0f)
        {
            pathFinder = std::make_unique<AStarPathFinder>(HeuristicType::CHEBYSHEV);
        };

        ~Agent() override = default;

        static constexpr const char* name()
        {
            return "Agent";
        }

        const char* getName() const override
        {
            return name();
        }

        void onAwake() override;

        /**
         * @brief Agent::update() method is used to calculate the desired velocity of this agent, using all current enabled modules
         * @param deltaTime
         * @param gameWorld Unused for Agent
         */
        void update(double deltaTime, const GameWorld& gameWorld) override;

        /**
         * @brief Uses all enabled modules to compute a Vector2 in which the agent should move.
         * The returning vector is the sum of each module * their weight.
         */
        Vector2 computeModulesForce();

        /**
         * @brief Computes the velocity the agent should move in this frame
         *
         *
         * @return A vector2 based off @c computeModuleForce(), clamped to @c maxSpeed
         */
        Vector2 computeDesiredVelocity();

        /**
         * @brief Adds a module to this agent to use during its calculations
         *
         * @tparam T Type of the module (must derive from @c BaseAgentModule)
         * @tparam Args Constructor arguments of the module
         * @param desiredWeight Weight of this module, this corresponds to how fast the agent moves in the computed direction of the module. Can be tweaked using @c setModuleWeight()
         * @param args Constructor arguments of the module
         * @return True if the module has been added succesfully, false if adding failed (no duplicates allowed)
         */
        template<typename T, typename... Args>
        bool addAgentModule(float desiredWeight, Args &&... args);

        /**
         * @brief Removes an existing module from this Agent
         * @tparam T The type to remove (must derive from BaseAgentModule)
         * @return True if the module was found and removed succesfully, false if the module was not found and removed.
         */
        template<typename T>
        bool removeAgentModule();

        /**
         * @brief Sets the weight of the desired module so a new value
         * @tparam T Type of module (Must derive from BaseAgentModule)
         * @param desiredWeight the new weight the module should have
         * @return true if the module was found and it's weight was changed, false otherwise
         */
        template<typename T>
        bool setModuleWeight(float desiredWeight) const;

        /**
         * @brief Retrieves the current weight of the desired module
         * @tparam T Type of module (Must derive from BaseAgentModule)
         * @return The current weight of the module. Returns 0 if module does not exist on this agent
         */
        template<typename T>
        [[nodiscard]] float getModuleWeight() const;

        /**
         * @brief sets the active status of the desired module, using @c ModuleStatus
         * @tparam T Type of module (Must derive from BaseAgentModule)
         * @param status Desired status of the module
         * @return true if module was found on this agent and the status was set, false otherwise
         */
        template<typename T>
        bool setModuleStatus(ModuleStatus status) const;

        /**
         * @brief Retrieves the @c ModuleStatus of the desired module
         * @tparam T Type of module (Must derive from BaseAgentModule)
         * @return The current status of the module
         */
        template<typename T>
        [[nodiscard]] ModuleStatus getModuleStatus() const;

        /**
         * @brief Checks whether this agent has the desired module
         * @tparam T Type of module (Must derive from BaseAgentModule)
         * @return true if this agent has the desired module, false otherwise
         */
        template<typename T>
        [[nodiscard]] bool hasAgentModule() const;

        /**
         * Retrieves the maxSpeed this agent is allowed to move
         * MaxSpeed is used to clamp the module forces to a maximum
         * @return the value of @c maxSpeed
         */
        [[nodiscard]] float getMaxSpeed() const;

        /**
         * @brief Sets the value of @c maxSpeed, which is used to clamp the module forces to a maximum
         * @param max the new max value
         */
        void setMaxSpeed(float max);

        /**
         * @brief Returns the number of modules this agent currently has
         */
        [[nodiscard]] size_t getModuleCount() const;

        /**
         * @brief Retrieves the value of @c arrivingDistance, used to determine if this agent has arrived at its destination
         * @return the current value of @c arrivingDistance
         */
        float getArrivingDistance() const;

        /**
         * @brief Sets the arriving distance of this agent
         * Arriving distance is used by some modules to determine if this agent has arrived at its desired target destination
         * @param distance
         */
        void setArrivingDistance(float distance);

        /**
         * @brief Retrieves the turn rate this agent rotates with towards its velocity
         * @return The current value of @c rotationTurnRate
         */
        float getRotationTurnRate() const;

        /**
         * @brief Sets the turn rate this agent rotates with towards its driving velocity
         *
         * @param value the new value, in degrees (0-360). Higher degrees corresponds to faster rotation speed, while lower values make the agent rotate more slowly
         */
        void setRotationTurnRate(float value);

        /**
         * @brief Retrieves the acceleration multiplier used for physics-driven agents.
         * This scales the acceleration based on @c maxSpeed.
         */
        float getAccelerationMultiplier() const;

        /**
         * @brief Sets the acceleration multiplier used for physics-driven agents.
         * Higher values make the agent converge to its desired velocity faster.
         */
        void setAccelerationMultiplier(float value);

        /**
         * @brief Retrieves the minimum acceleration used for physics-driven agents.
         * This acts as a floor so slow agents still respond promptly.
         */
        float getMinAcceleration() const;

        /**
         * @brief Sets the minimum acceleration used for physics-driven agents.
         */
        void setMinAcceleration(float value);

        /**
         * @brief Computes a path for this agent, using the scene's @c NavigationSystem
         *
         * This method does not return any other information about the path. To get the actual path,
         * use the @c getPathResult() method, which returns a @c PathResult containing information.
         *
         * Returns whether the retrieved path is valid (path >0, with total distance between points >0 as well)
         * @param target the goal of the path
         * @return True if the retrieved path is Valid, false otherwise
         */
        bool requestPath(const Vector2& target);

        /**
         * Returns the current active path of this agent.
         *
         * The path is stored in a @c PathResult, which can be used to retrieve the NavSurface-oriented points.
         * @return the current PathResult of this agent.
         */
        const PathResult& getCurrentPath() const;

        /**
         * @brief Utility method to determine if this agent has a valid path.
         * @return true if this agents current path stored in @c currentPath is valid.
         */
        bool hasPath() const;

    private:
        /// @brief Internal method used by other template methods that attempts to retrieve an BaseAgentModule on this Agent.
        /// Sets the value of @c out param to the ModuleData containing the module @c T
        template<typename T>
        bool tryGetAgentModule(ModuleData*& out) const;

        /// @brief Internal method used by other template methods that attempts to retrieve an BaseAgentModule on this Agent
        /// Sets the value of @c out param to the ModuleData containing the module @c T
        /// Sets the value of @c index to the ModuleData's index in the @c moduleDatas if the @c T module was found
        template<typename T>
        bool tryGetAgentModule(ModuleData*& out, size_t& index) const;

        /// @brief Vector containing all the ModuleDatas; Which store the Module, Weight, and status of a module. Internal use only
        std::vector<std::unique_ptr<ModuleData>> moduleDatas;

        ///@brief Current velocity this agent is moving in, internal use only
        Vector2 currentVelocity;

        ///@brief Max speed this agent is allowed to move with. Velocity of this agent gets clamped by this value. Internal access only, use get/set methods to gain access.
        float maxSpeed;

        ///@brief The distance to determine if an agent has arrived at its desired destination. Internal access only, use get/set methods to gain access.
        /// Used by some modules to compute their desired force
        float arrivingDistance;

        /// @brief The rate to turn the agent towards its current velocity with, in degrees (0 -360).
        float rotationTurnRate;

        /// @brief Multiplier that converts @c maxSpeed into acceleration strength for physics bodies.
        float accelerationMultiplier;

        /// @brief Minimum acceleration to keep low-speed agents responsive.
        float minAcceleration;

        /// @brief Pathfinder this agent wants to use for pathfinding.
        std::unique_ptr<IPathFinder> pathFinder;

        /// The current path this agent has stored.
        /// Note: An agent itself does nothing with a path, he only stores it so modules and other behaviours can make use of it.
        /// To get a new path to a target, use the @c requestPath() method
        PathResult currentPath;

        RigidBody* rigidBody = nullptr;
};

#include "AI/AgentImplementation.h"
