#pragma once
#include "Networking/ComponentType.h"
#include "Networking/Serialization/ISerializable.h"
#include "Networking/Serialization/Serialization.h"


class Transform;
class GameObject;

/**
 * @class Component
 * @brief Base class for all components attached to a GameObject.
 *
 * Components define modular behaviors for GameObjects, such as rendering, physics,
 * input handling, or custom logic. Each Component is associated with a GameObject
 * and can access its Transform for position, rotation, and scale.
 *
 */
class Component : public ISerializable
{
public:
	Component() : gameObject(nullptr), transform(nullptr)
	{
	};
	virtual ~Component() = 0;

	Component(const Component& other) = delete;
	Component(Component&& other) = delete;
	Component& operator=(const Component& other) = delete;
	Component& operator=(Component&& other) = delete;

	/**
	* @brief Sets the GameObject that this component lives on.
	* Also sets the @c transform field to the GameObject's Transform.
	* @param object Pointer to the GameObject to associate with this component.
	*/
	void setGameObject(GameObject* object);

	static constexpr const char* name() {
		return "component";
	}

	virtual const char* getName() const { return name(); }

	/**
	* @brief Adds a new component of type T to the owning GameObject.
	*
	* This is a template method that forwards the call to the GameObject's
	* addComponent method. Creates the component if it does not already exist.
	*
	* @tparam T Type of the component to add (must inherit from Component)
	* @return Pointer to the newly added component
	*/
	template <typename T>
	T* addComponent();

        /**
        * @brief Retrieves a component of type T from the owning GameObject.
        *
        * This is a template method that forwards the call to the GameObject's
        * getComponent method.
        *
        * @tparam T Type of the component to retrieve
        * @return Pointer to the component if found, nullptr otherwise
        */
        template <typename T>
        T* getComponent() const;

        /**
        * @brief Attempts to retrieve a component of type T from the owning GameObject.
        *
        * This is a template method that forwards the call to the GameObject's
        * tryGetComponent method.
        * Sets the output pointer to the component if it exists, otherwise sets it to nullptr.
        *
        * @tparam T Type of the component to retrieve
        * @param[out] out Reference to a pointer that will store the component
        * @return true if the component exists, false otherwise
        */
        template <typename T>
        bool tryGetComponent(T*& out) const;


        /**
         * @brief Called right before this Component gets destroyed
         *
         * Gets called internally right before the GameObject this component belongs to gets destroyed by the owning scene.
         *
         * Can be overridden to implement custom logic when the component gets destroyed.
         */
        virtual void onDestroy() {}

      /**
      * @brief Returns the GameObject that this component is attached to.
      * A Component is always attached to a GameObject.
      * @return Pointer to the owning GameObject.
      */
        GameObject* getGameObject() const;

	/**
	* @brief Returns the Transform of the GameObject this component belongs to.
	* @return Pointer to the associated GameObject's Transform.
	*/
	const Transform* getTransform() const;
	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;
protected:
	/// The @c GameObject this component is attached to, a component is always attached to a GameObject
	GameObject* gameObject;

	/// The @c Transform that is attached to the associated GameObject.
	Transform* transform;
};

/// Implementation of the template functions:
#include "../ComponentImplementation.inl"
