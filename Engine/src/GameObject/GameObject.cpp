#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "Component/Transform.h"
#include "Networking/Component/ComponentFactory.h"
#include "Networking/Serialization/Serialization.h"
#include "Scene/Scene.h"

#include <algorithm>
#include <iostream>

GameObject::GameObject()
{
	transform = std::make_unique<Transform>();
	transform->setGameObject(this);
	name = "GameObject";
	layer = 0;
	tag = "";
	isActive = true;
	isStatic = false;
	isDestroyed = false;
	scene = nullptr;
	parent = nullptr;
	children = {};
}


GameObject::~GameObject()
{
	// Clean up parent-child relationship to prevent dangling pointers
	if ( parent != nullptr )
	{
		parent->removeChild(this);
		parent = nullptr;
	}
	destroyAllComponents();
	transform = nullptr;

	// Clear parent pointers from all children to prevent dangling pointers
	for ( GameObject* child : children )
	{
		if ( child != nullptr )
		{
			child->parent = nullptr;
		}
	}
	children.clear();
}

bool GameObject::compareTag(const std::string& other) const
{
	return tag == other;
}

bool GameObject::hasComponent(const Component* comp) const
{
	if ( comp == nullptr ) return false;

	auto it = std::ranges::find_if(
		components, [&](const std::unique_ptr<Component>& component)
		{ return component.get() == comp; });

	return it != components.end();
}

void GameObject::removeComponent(Component* comp)
{
	if ( !comp || !hasComponent(comp) ) return;

	/// If its a behaviour, we also need to remove it from the behaviours
	/// vectors:
	if ( auto* behaviour = dynamic_cast<Behaviour*>(comp) )
	{
		behaviour->setEnabled(false);

		behaviours.erase(
			std::remove(behaviours.begin(), behaviours.end(), behaviour),
			behaviours.end());

		enabledBehaviours.erase(std::remove(enabledBehaviours.begin(),
											enabledBehaviours.end(), behaviour),
								enabledBehaviours.end()

		);
	}

	/// Remove the component from the components vector.
	auto it = std::ranges::find_if(
		components, [&](const std::unique_ptr<Component>& component)
		{ return component.get() == comp; });

	if ( it != components.end() )
	{
		it->get()->onDestroy();
		components.erase(it);
	}
}

const std::vector<Behaviour*>& GameObject::getAllBehaviours() const
{
	return behaviours;
}

const std::vector<Behaviour*>& GameObject::getEnabledBehaviours()
{
	enabledBehaviours.clear();

	for ( Behaviour* behaviour : behaviours )
	{
		if ( behaviour != nullptr && behaviour->getIsEnabled() )
			enabledBehaviours.push_back(behaviour);
	}

	return enabledBehaviours;
}

void GameObject::destroy()
{
	if ( isDestroyed ) return;

	isDestroyed = true;
	setActive(false);

	disableAllBehaviours();

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
    disableAllBehaviours();
}


void GameObject::onSceneDestroy()
{
	destroyAllComponents();
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
	return static_cast<int>(components.size());
}

bool GameObject::getIsDestroyed() const
{
	return isDestroyed;
}

int GameObject::getSceneId() const
{
	if ( scene == nullptr ) return -1;

	return sceneId;
}

void GameObject::setName(const std::string& newName)
{
	name = newName;
}

void GameObject::setLayer(const int newLayer)
{
	layer = newLayer;
}

void GameObject::setTag(const std::string& newTag)
{
	tag = newTag;
}

void GameObject::setActive(const bool value)
{
	if ( isActive == value ) return;

	isActive = value;
}

void GameObject::setIsStatic(const bool value)
{
	if ( isStatic == value ) return;

	isStatic = value;
}

void GameObject::setScene(Scene& newScene)
{
	scene = &newScene;
}

Scene* GameObject::getScene() const
{
	return scene;
}

void GameObject::setBehavioursEnabled(const bool value) const
{
	if ( value )
	{
		enableAllBehaviours();
	}
	else
	{
		disableAllBehaviours();
	}
}

void GameObject::setParent(GameObject* newParent)
{
	if ( parent == newParent ) return;

	GameObject* oldParent = parent;
	Transform* newParentTransform =
		newParent != nullptr ? newParent->getTransform() : nullptr;

	// Update Transform first; it will reject circular references
	if ( !transform->setParent(newParentTransform) ) return;

	// Remove from old parent's children list
	if ( oldParent != nullptr )
	{
		oldParent->removeChild(this);
	}

	// Update parent
	parent = newParent;

	// Add to new parent's children list
	if ( parent != nullptr )
	{
		parent->addChild(this);
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

void GameObject::markTransformDirty()
{
	// Mark this GameObject's Transform as dirty
	if ( transform != nullptr )
	{
		transform->markDirtyLocal();
	}

	// Mark all children's Transforms as dirty (their world matrices depend on
	// this transform)
	for ( GameObject* child : children )
	{
		if ( child != nullptr )
		{
			child->markTransformDirty();
		}
	}
}

void GameObject::serialize(WriteArchive& archive) const
{
	std::string n = name;
	archive.process(n);

	// Transform
	Transform* t = getTransform();
	float posX = t->getPosition().x;
	float posY = t->getPosition().y;
	double rotation = t->getRotationAngle();
	float scaleX = t->getScale().x;
	float scaleY = t->getScale().y;

	archive.process(posX);
	archive.process(posY);
	archive.process(rotation);
	archive.process(scaleX);
	archive.process(scaleY);

	// Count
	uint32_t count = 0;
	for ( const auto& comp : getComponents() )
	{
		if ( dynamic_cast<Transform*>(comp.get()) ) continue;
		const char* typeName = comp->getName();
		if ( typeName && ComponentFactory::instance().isRegistered(typeName) )
		{
			count++;
		}
	}
	archive.process(count);

	// Components
	for ( const auto& comp : getComponents() )
	{
		if ( dynamic_cast<Transform*>(comp.get()) ) continue;
		const char* typeName = comp->getName();
		if ( !typeName || !ComponentFactory::instance().isRegistered(typeName) )
		{
			continue;
		}

		std::string typeNameStr(typeName);
		archive.process(typeNameStr);
		comp->serialize(archive);
	}
}

void GameObject::deserialize(ReadArchive& archive)
{
	archive.process(name);

	float posX, posY;
	double rotation;
	float scaleX, scaleY;

	archive.process(posX);
	archive.process(posY);
	archive.process(rotation);
	archive.process(scaleX);
	archive.process(scaleY);

	getTransform()->setPosition({posX, posY});
	getTransform()->setRotationAngle(rotation);
	getTransform()->setScale({scaleX, scaleY});

	uint32_t count;
	archive.process(count);

	for ( uint32_t i = 0; i < count; ++i )
	{
		std::string typeName;
		archive.process(typeName);

		if ( Component* existing = getComponentByTypeName(typeName) )
		{
			existing->deserialize(archive);
		}
		else
		{
			auto comp = ComponentFactory::instance().create(typeName);
			if ( !comp )
			{
				std::cerr << "[GameObject] Unknown component: " << typeName
						  << std::endl;
				break;
			}
			comp->deserialize(archive);
			internalAddComponent(std::move(comp));
		}
	}
	fixupPointersAfterClone();
}

void GameObject::fixupPointersAfterClone()
{
	for ( auto& component : components )
	{
		if ( component )
		{
			component->setGameObject(this);
		}
	}
}

std::unique_ptr<GameObject> GameObject::clone() const
{
	WriteArchive writer;
	serialize(writer);

	std::vector<std::byte> bytes = writer.getBytes();
	CerealReadArchive reader(bytes.data(), bytes.size());

	auto cloned = std::make_unique<GameObject>();
	cloned->deserialize(reader);

	for ( auto& component : cloned->components )
	{
		if ( component )
		{
			component->setGameObject(cloned.get());
		}
	}

	for ( auto& behaviour : cloned->behaviours )
	{
		if ( behaviour )
		{
			behaviour->setGameObject(cloned.get());
		}
	}

	return cloned;
}

void GameObject::copyStateFrom(const GameObject& source)
{
	WriteArchive writeArchive;
	source.serialize(writeArchive);

	std::vector<std::byte> bytes = writeArchive.getBytes();
	ReadArchive readArchive(bytes.data(), bytes.size());
	deserialize(readArchive);
}


void GameObject::enableAllBehaviours() const
{
	for ( auto& behaviour : behaviours )
	{
		behaviour->setEnabled(true);
	}
}

void GameObject::disableAllBehaviours() const
{
	for ( auto& behaviour : behaviours )
	{
		behaviour->setEnabled(false);
	}
}

void GameObject::destroyAllComponents()
{
	while ( !components.empty() )
	{
		removeComponent(components.back().get());
	}
}

Component* GameObject::getComponentByTypeName(const std::string& typeName) const
{
	for ( const auto& comp : getComponents() )
	{
		const char* compTypeName = comp->getName();
		if ( compTypeName && typeName == compTypeName )
		{
			return comp.get();
		}
	}
	return nullptr;
}

const std::vector<std::unique_ptr<Component>>& GameObject::getComponents() const
{
	return components;
}

ObjectHandle GameObject::getGameObjectHandle() const
{
    return gameObjectHandle;
}

void GameObject::setGameObjectHandle(const ObjectHandle handle)
{
    gameObjectHandle = handle;
}

void GameObject::internalAddComponent(std::unique_ptr<Component> component)
{
	if ( component != nullptr )
	{
		component->setGameObject(this);

		if ( auto* behaviour = dynamic_cast<Behaviour*>(component.get()) )
		{
			behaviours.push_back(behaviour);
		}

		components.push_back(std::move(component));
	}
}
