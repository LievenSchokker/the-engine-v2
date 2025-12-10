//
// Created by samle on 10/11/2025.
//
#include "GameObject/GameObject.h"

#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "Component/Transform.h"
#include "GameObject/ScenePlaceholder.h"
#include "Scene/Scene.h"

#include <algorithm>

GameObject::GameObject()
{
	componentManager = std::make_unique<ComponentManager>(this);
	transform = std::make_unique<Transform>();
	// Set the gameObject on Transform so it can access children for markDirty()
	transform->setGameObject(this);
	name = "GameObject";
	layer = 0;
	tag = "";
	isActive = true;
	isStatic = false;
	isDestroyed = false;
	parent = nullptr;

	/// TODO: Remove this line:
	scene = nullptr;
}

GameObject::~GameObject()
{
	componentManager->destroyAllComponents();
	transform = nullptr;
}

bool GameObject::compareTag(const std::string& other)
{
	return tag == other;
}

bool GameObject::hasComponent(Component* comp) const
{
	return componentManager->hasComponent(comp);
}

void GameObject::removeComponent(Component* comp)
{
	componentManager->removeComponent(comp);
}

const std::vector<Behaviour*>& GameObject::getAllBehaviours() const
{
	return componentManager->getAllBehaviours();
}

const std::vector<Behaviour*>& GameObject::getEnabledBehaviours() const
{
	return componentManager->getEnabledBehaviours();
}

ComponentManager* GameObject::getComponentManager() const
{
	return componentManager.get();
}

void GameObject::destroy()
{
	if ( isDestroyed ) return;

	isDestroyed = true;
	setActive(false);

	componentManager->disableAllBehaviours();

	// Remove parent relationship before destroying
	setParent(nullptr);

	// Destroy all children
	// Create a copy of children vector since destroy() will modify it
	std::vector<GameObject*> childrenCopy = children;
	for ( GameObject* child : childrenCopy )
	{
		if ( child != nullptr )
		{
			child->destroy();
		}
	}

	if ( scene != nullptr ) scene->queueDestroy(this);
}

void GameObject::onSceneDestroy()
{
	componentManager->destroyAllComponents();
}

Transform* GameObject::getTransform() const
{
	return transform.get();
}

std::string GameObject::getName() const
{
	return name;
}

int GameObject::getLayer() const
{
	return layer;
}

std::string GameObject::getTag() const
{
	return tag;
}

bool GameObject::getIsActive() const
{
	return isActive;
}

bool GameObject::getIsStatic() const
{
	return isStatic;
}

int GameObject::getComponentCount() const
{
	return componentManager->getComponentCount();
}

bool GameObject::getIsDestroyed() const
{
	return isDestroyed;
}

void GameObject::setName(const std::string& newName)
{
	name = std::move(newName);
}

void GameObject::setLayer(int newLayer)
{
	layer = newLayer;
}

void GameObject::setTag(const std::string& newTag)
{
	tag = std::move(newTag);
}

void GameObject::setActive(bool value)
{
	if ( isActive == value ) return;

	isActive = value;
}

void GameObject::setIsStatic(bool value)
{
	if ( isStatic == value ) return;

	isStatic = value;
}

void GameObject::setScene(Scene& newScene)
{
	scene = &newScene;
}

void GameObject::setBehavioursEnabled(const bool value) const
{
	if ( value )
	{
		componentManager->enableAllBehaviours();
	}
	else
	{
		componentManager->disableAllBehaviours();
	}
}

void GameObject::setParent(GameObject* newParent)
{
	if ( parent == newParent ) return;

	// Prevent circular parenting
	if ( newParent != nullptr )
	{
		GameObject* check = newParent->getParent();
		while ( check != nullptr )
		{
			if ( check == this )
			{
				// Circular reference detected, don't set parent
				return;
			}
			check = check->getParent();
		}
	}

	// Remove from old parent's children list
	if ( parent != nullptr )
	{
		parent->removeChild(this);
	}

	// Update parent
	parent = newParent;

	// Add to new parent's children list
	if ( parent != nullptr )
	{
		parent->addChild(this);
		// Update Transform's parent reference
		transform->setParent(parent->getTransform());
	}
	else
	{
		// No parent, remove Transform's parent reference
		transform->setParent(nullptr);
	}
}

GameObject* GameObject::getParent() const
{
	return parent;
}

const std::vector<GameObject*>& GameObject::getChildren() const
{
	return children;
}

int GameObject::getChildCount() const
{
	return static_cast<int>(children.size());
}

void GameObject::removeChild(GameObject* child)
{
	children.erase(std::remove(children.begin(), children.end(), child),
				   children.end());
}

void GameObject::addChild(GameObject* child)
{
	// Check if already a child
	if ( std::find(children.begin(), children.end(), child) == children.end() )
	{
		children.push_back(child);
	}
}