//
// Created by samle on 10/11/2025.
//

#pragma once
#include "Scene/Scene.h"
class Transform;
class Component;
class Behaviour;
class ComponentManager;
class ScenePlaceholder;

#include <memory>
#include <string>
#include <vector>

/**
 * @class GameObject
 * @brief Represents an object in the game world.
 *
 * GameObjects are the core entities in the game engine. They can have multiple
 * Components that define their behavior and functionality.
 *
 * Each GameObject has a @c componentManager to manage its components,
 * and a @c transform to define its place in the game world.
 * A GameObject can further be organized using
 * tags, layers, and active/static state.
 *
 */
class GameObject
{
    public:
        /**
         * @brief Constructs a new GameObject.
         *
         * Initializes the @c transform and @c componentManager.
         */
        GameObject();


        /**
         * @brief Destructor. Cleans up all components and resources.
         */
        ~GameObject();


        /**
        * @brief Adds a component of type T to this object's @c componentManager.
        *
        * @tparam T Type of the component to add (must inherit from Component)
        * @return Pointer to the newly added component
        */
        template<typename T>
        T* addComponent();


       /**
       * @brief Retrieves a component of type T from this GameObject's @c componentManager.
       *
       * Returns nullptr if the component does not exist.
       *
       * @tparam T Type of the component to retrieve
       * @return Pointer to the component if found, nullptr otherwise
       */
        template<typename T>
        T* getComponent() const;


        /**
        * @brief Attempts to retrieve a component of type T from this GameObject's @c componentManager.
        *
        * Sets the output pointer to the component if it exists, otherwise sets it to nullptr.
        *
        * @tparam T Type of the component to retrieve
        * @param[out] out Reference to a pointer that will store the component
        * @return true if the component exists, false otherwise
        */
        template<typename T>
        bool tryGetComponent(T*& out) const;


        /**
        * @brief Retrieves a component of type T or adds it if it does not exist.
        *
        * @tparam T Type of the component
        * @return Pointer to the existing or newly added component
        */
        template<typename T>
        T* getOrAddComponent();


        /**
        * @brief Removes a component of type T from this GameObject's @c componentManager
        *
        * @tparam T Type of the component to remove
        */
        template<typename T>
        void removeComponent();


        /**
        * @brief Removes the given component from this GameObject's @c componentManager
        *
        * @tparam comp  @c Component to remove
        */
        void removeComponent(Component* comp);


        /**
        * @brief Checks if the GameObject has a component of type T.
        * @param T type to check for
        * @return true if the component exists, false otherwise
        */
        template<typename T>
        bool hasComponent() const;


        /**
        * @brief Checks if the GameObject has the specified component.
        * @param comp Pointer to the component to check
        * @return true if the component exists, false otherwise
        */
        bool hasComponent(Component* comp) const;


        /**
        * @brief Compares the GameObject's tag to another string.
        * @param other Tag to compare with
        * @return true if the tags match, false otherwise
        */
        bool compareTag(const std::string& other);

        const std::vector<Behaviour*>& getAllBehaviours() const;

        /**
        * @brief Returns all active Behaviour components attached to this GameObject.
        * @return Vector of pointers to active Behaviour components
        */
        const std::vector<Behaviour*>& getEnabledBehaviours() const;


        /**
        * @brief Returns the Transform of this GameObject.
        * @return Pointer to the Transform
        */
        Transform* getTransform() const;


        /**
        * @brief Returns the name of this GameObject.
        * @return Name as a std::string
        */
        std::string getName() const;


        /**
        * @brief Returns the layer of this GameObject.
        * @return Layer as an int
        */
        int getLayer() const;


        /**
        * @brief Returns the tag of this GameObject.
        * @return Tag as a std::string
        */
        std::string getTag() const;

        /**
        * @brief Returns whether the GameObject is active.
        * @return true if active, false otherwise
        */
        bool getIsActive() const;


        /**
        * @brief Returns whether the GameObject is static.
        * @return true if static, false otherwise
        */
        bool getIsStatic() const;


        /**
        * @brief Returns the number of components attached to this GameObject.
        * @return Number of components
        */
        int getComponentCount() const;


        /**
        * @brief Sets the name of this GameObject.
        * @param newName New name as a std::string
        */
        void setName(const std::string& newName);


       /**
        * @brief Sets the layer of this GameObject.
        * @param newLayer Layer index as an int
        */
        void setLayer(int newLayer);


        /**
        * @brief Sets the tag of this GameObject.
        * @param newTag Tag as a std::string
        */
        void setTag(const std::string& newTag);


        /**
        * @brief Sets whether this GameObject is active.
        * @param value true to activate, false to deactivate
        */
        void setActive(bool value);


        /**
        * @brief Sets whether this GameObject is static.
        * @param value true to mark as static, false otherwise
        */
        void setIsStatic(bool value);

        /**
         * @brief sets the scene this GameObject is currently owned by.
         *
         * Used to queue up this object to be destroyed by the scene owner upon calling @c destroy()
         * @param newScene new owning scene of this GameObject
         */
        void setScene(Scene& newScene);

        void setBehavioursEnabled(bool value) const;

        /**
         * @brief Helper function to get the @c componentManager directly,
         * used primarily for testing (unit tests)
         * @return the @c componentManager of this GameObject.
         */
        ComponentManager* getComponentManager() const;

        /**
         * @brief Marks this object and its components to be destroyed by the Scene.
         *
         * @c isDestroyed will be set to true, but the object and its components will be destroyed by the @c scene this object belongs to.
         *
         */
        void destroy();

        /**
         * Called by @c scenePlaceholder after this GameObject has been marked for destroyment by @c destroy()
         */
        void onSceneDestroy();

        /**
         * Checks whether this GameObject has been queued to be destroyed.
         *
         * Use this to determine if a GameObject is still present in memory, but should be removed at the end of the frame.
         * @return the value of @c isDestroyed, set true in @c destroy() method.
         */
        bool getIsDestroyed() const;

    private:
        std::unique_ptr<ComponentManager> componentManager;
        std::unique_ptr<Transform> transform;
         Scene* scene;
        std::string name;
        int layer;
        std::string tag;

        bool isActive;
        bool isStatic;
        bool isDestroyed;
};

#include "GameObjectImplementation.inl"
