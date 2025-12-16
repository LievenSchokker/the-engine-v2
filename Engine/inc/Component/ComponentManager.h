#pragma once


class Component;
class GameObject;
class Behaviour;
class Transform;

#include <vector>
#include <memory>

/**
 * @class ComponentManager
 * @brief Manages all components attached to a single GameObject.
 *
 * ComponentManager is responsible for adding, retrieving, removing, and
 * managing the lifecycle of components for a specific GameObject.
 * It allows easy access to behaviours and handles activation/deactivation
 * of components.
 */
class ComponentManager
{
public:
	/**
	 * @brief Constructs a ComponentManager for the given GameObject.
	 * @param parent_ Pointer to the owning GameObject
	 */
	ComponentManager(GameObject* parent_)
		: gameObject(parent_)
	{
	}

	/**
	* @brief Destructor. Cleans up all components managed by this manager.
	*/
	~ComponentManager();

	/**
   * @brief Adds a new component of type T to the @c components.
   *
   * @tparam T Type of the component to add (must inherit from Component)
   * @return Pointer to the newly added component
   */
	template <typename T, typename... Args>
	T* addComponent(Args&&... args);

	/**
	* @brief Retrieves a component of type T from  @c components, if it exists.
	*
	* @tparam T Type of the component to get (must inherit from Component)
	* @return Pointer to the retrieved component
	*/
	template <typename T>
	T* getComponent() const;

	/**
	* @brief Attempts to retrieve a component of type T from @c components.
	*
	* Sets the output pointer to the component if it exists, otherwise sets it to nullptr.
	*
	* @tparam T Type of the component to retrieve
	* @param[out] out Reference to a pointer that will store the component
	* @return true if the component exists, false otherwise
	*/
	template <typename T>
	bool tryGetComponent(T*& out) const;

	/**
   * @brief Removes a component of type T from @c components.
   *
   * Does NOT throw a warning or error if T doesn't exist in @c components
   *
   * @tparam T Type of the component to remove
   */
	template <typename T>
	void removeComponent();

	/**
	* @brief Removes the specified component from the GameObject.
	* @param component Pointer to the component to remove
	*/
	void removeComponent(Component* component);

	/**
	 * @brief removes and destroys all components stored in @c components
	 */
	void destroyAllComponents();

	/**
	* @brief Checks if @c components contains a component of type T.
	*
	* @tparam T Type of the component
	* @return true if @c components contains T, false otherwise
	*/
	template <typename T>
	bool
	hasComponent() const;

	/**
	* @brief Checks if @c components contains the given component.
	*
	* @tparam Pointer to the component to check
	* @return true if @c components contains the component, false otherwise
	*/
	bool hasComponent(const Component* comp) const;


	/**
	* @brief Returns all behaviours stored by @c behaviours
	* @return Vector of pointers to all exisiting Behaviour components
	*/
	const std::vector<Behaviour*>& getAllBehaviours() const;

	/**
	 * Gets all the behaviours that are currently enabled.
	 * @return a const vector of pointers to the enabled behaviours on this gameobject.
	 */
	const std::vector<Behaviour*>& getEnabledBehaviours();

	/**
   * @brief Activates all @c Behaviour in @c components.
   */
	void enableAllBehaviours() const;

	/**
	* @brief Deactivates all @c Behaviour in @c components.
	*/
	void disableAllBehaviours() const;

	/**
	* @brief Returns the total number of components stored inside @c components
	* @return Number of components
	*/
	size_t getComponentCount() const;
	const std::vector<std::unique_ptr<Component>>& getComponents() const;

	void addComponent(std::unique_ptr<Component> component);

	/**
	 * @brief Retrieves all components of type T from @c components.
	 *
	 * @tparam T Type of the components to get (must inherit from Component)
	 * @return Vector of pointers to all matching components
	 */
	template <typename T>
	std::vector<T*> getAllComponentsOfType() const;

   private:
	/// Helper function to iterate through @c components
	template <typename T>
	std::vector<std::unique_ptr<Component>>::iterator getComponentIterator();
	/// Helper function to iterate through @c components, const version.
	template <typename T>
	std::vector<std::unique_ptr<Component>>::const_iterator
	getComponentIterator() const;
private:
	/// The @c GameObject this ComponentManager belongs to
	GameObject* const gameObject;
	/// All components stored by this object
	std::vector<std::unique_ptr<Component>> components;

	/// All behaviours of this gameobject
	std::vector<Behaviour*> behaviours;

	/// All enabled behaviours of this gameobject.
	std::vector<Behaviour*> enabledBehaviours;
};

/// Template implementations:
#include "BaseComponentTypes/Component.h"
#include "ComponentManagerImplementation.inl"