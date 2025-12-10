#pragma once


#include "Component/BaseComponentTypes/Component.h"

class GameWorld;
/**
 * @brief This class serves as an abstract component that custom behaviour can derive from.
 *
 * Implements lifetime functions that get called by the Engine internally
 *
 * Behaviour differs from Component because they can be enabled and disabled, and implement lifetime functions.
 */
class Behaviour: virtual public Component
{
public:
	Behaviour() :
		isEnabled(true), hasAwakened(false), hasStarted(false)
	{
	}

	ComponentType getComponentType() const override
	{
		return ComponentType::Unknown;
	}
	/**
	 * @brief Pure virtual destructor makes this class Abstract.
	 */
        virtual ~Behaviour() = 0;

        /**
         * @brief  Called when this Behaviour is first loaded:
         * - Only on an active @c GameObject.
         * - Regardless of @c isEnabled status
         * - Outside of this constructor
         *
         * Use Awake instead of the constructor for initialisation, always called by the engine once per scene instance.
         * Awake is always called by the engine before @c onEnable() and @c start() functions.
         */
        void awake();

        /**
         * Callback for when @c awake() is called by the engine on this behaviour.
         */
        virtual void onAwake() {};

        /**
         * @brief Called when this component is enabled:
         * - Only on active GameObjects
         * - after @c isEnabled turns true
         */
        virtual void onEnable() {}

        /**
         * @brief start is called after @c awake, and before the first @c update call.
         *
         * Start on any component is guaranteed to be called by the engine after all awake functions on all behaviours in the scene have been called,
         *
         * Start is only called on enabled behaviours.
         */
        void start();

        /**
         * Callback when @ref start() is called by the engine on this behaviour.
         */
        virtual void onStart() {};

        /**
         * Update is called every frame when:
         * - @c isActiveAndEnabled == true, meaning the GameObject is active, and the Behaviour is enabled
         *
         * @param deltaTime Time elapsed since last update in seconds
         */
        virtual void update(float deltaTime, GameWorld* world) {};

        /**
         * @brief fixedUpdate is called at regular and fixed intervals as part of the engine's physics loop.
         *
         * Use fixedUpdate to perform physics calculations, like applying force to a Rigidbody.
         *
         * FixedUpdate may be called zero, one or multiple times per frame depending on the frame rate of the simulation,
         * to ensure consistent and deterministic physics calculations, regardless of rendering speed.
         */
        virtual void fixedUpdate() {};

        /**
        * @brief Called when this component is disabled:
        * - Only on active GameObjects
        * - after @c isEnabled turns false.
        */
        virtual void onDisable() {}


        /**
         * @brief Called when this Behaviour's @c GameObject gets destroyed.
         */
        void onDestroy() override;


        /**
         * @brief Sets the @c isEnabled field of this Behaviour
         * @param value new value to set
         */
        void setEnabled(bool value);


        /**
         * @brief Retrieves the state of the @c isEnabled field
         * @return whether this Behaviour is enabled, value of @c isEnabled
         */
        bool getIsEnabled() const;


        /**
         * @brief Checks whether this Behaviour is enabled, and its associated GameObject is active.
         * @return whether this behaviour is Enabled and on an active GameObject.
         */
        bool getIsActiveAndEnabled() const;

        /**
         * #brief Checks whether awake has been called yet
         * @return true if this behaviour's awake function has been called, false otherwise
         */
        bool getHasAwakened() const;


        /**
        * #brief Checks whether start has been called yet
        * @return true if this behaviour's start function has been called, false otherwise
        */
        bool getHasStarted() const;

    private:
        /// Enabled components are Updated, disabled Beahviours are not.
        bool isEnabled;

        /// Keeps track if awake has been called for this Behaviour.
        bool hasAwakened;

        /// Keeps track if start function has been called for this behaviour.
        bool hasStarted;
};
