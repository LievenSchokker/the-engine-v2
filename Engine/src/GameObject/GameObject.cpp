//
// Created by samle on 10/11/2025.
//
#include "GameObject/GameObject.h"

#include "Game.h"
#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "Component/Transform.h"
#include "GameObject/ScenePlaceholder.h"
#include "Networking/Component/ComponentFactory.h"
#include "Networking/Serialization/Serialization.h"
#include "Scene/Scene.h"

GameObject::GameObject()
{
    componentManager = std::make_unique<ComponentManager>(this);
    transform = std::make_unique<Transform>();
    transform->setGameObject(this);
    name = "GameObject";
    layer = 0;
    tag = "";
    isActive = true;
    isStatic = false;
    isDestroyed = false;
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


bool GameObject::hasComponent(Component *comp) const
{
    return componentManager->hasComponent(comp);
}


void GameObject::removeComponent(Component *comp)
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
    if (isDestroyed)
        return;

    isDestroyed = true;
    setActive(false);

    componentManager->disableAllBehaviours();

    if (scene != nullptr)
        scene->queueDestroy(this);
}


void GameObject::onSceneDestroy()
{
    componentManager->destroyAllComponents();
}


Transform *GameObject::getTransform() const
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

int GameObject::getSceneId() const
{
    if (scene == nullptr)
        return -1;

    return sceneId;
}



void GameObject::setName(const std::string &newName)
{
    name = std::move(newName);
}


void GameObject::setLayer(int newLayer)
{
    layer = newLayer;
}


void GameObject::setTag(const std::string &newTag)
{
    tag = std::move(newTag);
}


void GameObject::setActive(bool value)
{
    if (isActive == value)
        return;

    isActive = value;
}


void GameObject::setIsStatic(bool value)
{
    if (isStatic == value)
        return;

    isStatic = value;
}


void GameObject::setScene(Scene& newScene)
{
    scene = &newScene;
    sceneId = scene->getSceneId(*this);
}

Scene* GameObject::getScene() const
{
    return scene;
}


void GameObject::setBehavioursEnabled(const bool value) const
{
    if (value)
    {
        componentManager->enableAllBehaviours();
    }
    else
    {
        componentManager->disableAllBehaviours();
    }
}

void GameObject::serialize(WriteArchive& archive) const
{
	std::string n = name;
	archive.process(n);

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

	uint32_t count = 0;
	for (const auto& comp : componentManager->getComponents())
	{
		if (dynamic_cast<Transform*>(comp.get())) continue;

		ComponentType type = comp->getComponentType();

		if (ComponentFactory::instance().isRegistered(type))
		{
			count++;
		}
	}
	archive.process(count);

	for (const auto& comp : componentManager->getComponents())
	{
		if (dynamic_cast<Transform*>(comp.get())) continue;

		ComponentType type = comp->getComponentType();
		if (!ComponentFactory::instance().isRegistered(type))
		{
			continue;
		}

		uint32_t typeId = static_cast<uint32_t>(type);
		archive.process(typeId);
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

	for (uint32_t i = 0; i < count; ++i)
	{
		uint32_t typeId;
		archive.process(typeId);
		ComponentType type = static_cast<ComponentType>(typeId);

		// Try to find existing component first
		Component* existing = getComponentByType(type);

		if (existing)
		{
			// Update existing component
			existing->deserialize(archive);
		}
		else
		{
			// Create new component only if it doesn't exist
			auto comp = ComponentFactory::instance().create(type);
			if (!comp)
			{
				break;
			}

			comp->deserialize(archive);
			componentManager->addComponent(std::move(comp));
		}
	}
}

std::unique_ptr<GameObject> GameObject::clone() const
{
    // Serialize to bytes
    WriteArchive writer;
    serialize(writer);

    // Deserialize into new object
    std::vector<std::byte> bytes = writer.getBytes();
    CerealReadArchive reader(bytes.data(), bytes.size());

    auto cloned = std::make_unique<GameObject>();
    cloned->deserialize(reader);

    return cloned;
}

Component* GameObject::getComponentByType(ComponentType type) const
{
	for (const auto& comp : componentManager->getComponents())
	{
		if (comp->getComponentType() == type)
		{
			return comp.get();
		}
	}
	return nullptr;
}

void GameObject::copyStateFrom(const GameObject& source)
{

	WriteArchive writeArchive;
	source.serialize(writeArchive);

	std::vector<std::byte> bytes = writeArchive.getBytes();
	ReadArchive readArchive(bytes.data(), bytes.size());
	deserialize(readArchive);
}