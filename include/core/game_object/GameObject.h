//
// Created by samle on 10/11/2025.
//

#pragma once


class Transform;
class Component;
class Behaviour;
class ComponentManager;

#include <memory>
#include <string>
#include <vector>


class GameObject
{
    public:
        GameObject();
        ~GameObject();

        template<typename T>
        T* addComponent();
        template<typename T>
        T* getComponent();
        template<typename T>
        bool tryGetComponent(T*& out) const;
        template<typename T>
        T* getOrAddComponent();
        template<typename T>
        void removeComponent();

        bool hasComponent(Component* comp);
        void removeComponent(Component* comp);
        bool compareTag(std::string other);

        ComponentManager* getComponentManager() const;
        std::vector<Behaviour*> getActiveBehaviours() const;
        Transform* getTransform() const;
        std::string getName() const;
        int getLayer() const;
        std::string getTag() const;
        bool getIsActive() const;
        bool getIsStatic() const;
        int getComponentCount() const;

        void setName(const std::string newName);
        void setLayer(int newLayer);
        void setTag(const std::string newTag);
        void setIsActive(bool value);
        void setIsStatic(bool value);

    private:
        std::unique_ptr<ComponentManager> componentManager;
        std::unique_ptr<Transform> transform;
        std::string name;
        int layer;
        std::string tag;
        bool isActive;
        bool isStatic;
};

#include "GameObjectImplementation.inl"
