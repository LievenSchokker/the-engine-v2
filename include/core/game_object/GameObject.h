//
// Created by samle on 10/11/2025.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

class Transform;
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
        void addComponent();

        template<typename T>
        T* getComponent();

        template<typename T>
        bool tryGetComponent();

        template<typename T>
        T* getOrAddComponent();

        template<typename  T>
        void removeComponent();

        /// Getters:
        ComponentManager* getComponentManager() const;

        Transform* getTransform() const;

        std::string getName() const;

        int getLayer() const;

        std::string getTag() const;

        bool getIsActive() const;

        bool getIsStatic() const;

        /// Setters:
        void setName(std::string newName);

        void setLayer(int newLayer);

        void setTag(std::string newTag);

        void setIsActive(bool value);

        void setIsStatic(bool value);

    private:
        /// Fields:
        std::unique_ptr<ComponentManager> componentManager = nullptr;
        std::unique_ptr<Transform> transform = nullptr;
        std::string name;
        int layer;
        std::string tag;
        bool isActive;
        bool isStatic;
};

#include "GameObjectImplementation.h"