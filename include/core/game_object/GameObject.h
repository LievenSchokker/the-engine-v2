//
// Created by samle on 10/11/2025.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

class Transform;
class Component;
class Behaviour;
class ComponentManager;


class GameObject
{
    public:
        GameObject();

        ~GameObject();

        bool compareTag(std::string other);

        std::vector<Behaviour*> getActiveBehaviours() const;

        /// Template functions:
        template<typename T>
        T* addComponent();

        template<typename T>
        T* getComponent();

        template<typename T>
        bool tryGetComponent(T*& out);

        template<typename T>
        T* getOrAddComponent();

        template<typename  T>
        void removeComponent();

        bool hasComponent(Component* comp) const;
        void addComponent(Component* comp);
        void removeComponent(Component* comp);

        ComponentManager* getComponentManager() const;

        Transform* getTransform() const;

        std::string getName() const;

        int getLayer() const;

        std::string getTag() const;

        bool getIsActive() const;

        bool getIsStatic() const;

        int getComponentCount() const;

        /// Setters:
        void setName(std::string newName);

        void setLayer(int newLayer);

        void setTag(std::string newTag);

        void setIsActive(bool value);

        void setIsStatic(bool value);

    private:
        /// Fields:
        std::unique_ptr<ComponentManager> componentManager;
        std::unique_ptr<Transform> transform;
        std::string name;
        int layer;
        std::string tag;
        bool isActive;
        bool isStatic;
};

#include "GameObjectImplementation.inl"
