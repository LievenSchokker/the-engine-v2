#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "Component/Transform.h"
#include "Networking/Component/ComponentFactory.h"
#include "Networking/Serialization/Serialization.h"
#include "Scene/Scene.h"

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
}


GameObject::~GameObject()
{
    destroyAllComponents();
    transform = nullptr;
}


bool GameObject::compareTag(const std::string& other) const
{
    return tag == other;
}


bool GameObject::hasComponent(const Component* comp) const
{
    if (comp == nullptr)
        return false;

    auto it = std::ranges::find_if(components,
                                   [&](const std::unique_ptr<Component>& component)
                                   {
                                       return component.get() == comp;
                                   });

    return it != components.end();
}


void GameObject::removeComponent(Component* comp)
{
    if (!comp || !hasComponent(comp))
        return;

    /// If its a behaviour, we also need to remove it from the behaviours vectors:
    if (auto* behaviour = dynamic_cast<Behaviour*>(comp))
    {
        behaviour->setEnabled(false);

        behaviours.erase(
            std::remove(behaviours.begin(), behaviours.end(), behaviour),
            behaviours.end()
        );

        enabledBehaviours.erase(
            std::remove(enabledBehaviours.begin(), enabledBehaviours.end(), behaviour),
            enabledBehaviours.end()

        );
    }

    /// Remove the component from the components vector.
    auto it = std::ranges::find_if(components,
                                   [&](const std::unique_ptr<Component>& component)
                                   {
                                       return component.get() == comp;
                                   });

    if (it != components.end())
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

    for (Behaviour* behaviour : behaviours)
    {
        if (behaviour != nullptr && behaviour->getIsEnabled())
            enabledBehaviours.push_back(behaviour);
    }

    return enabledBehaviours;
}

void GameObject::destroy()
{
    if (isDestroyed)
        return;

    isDestroyed = true;
    setActive(false);

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
    if (scene == nullptr)
        return -1;

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
    if (isActive == value)
        return;

    isActive = value;
}


void GameObject::setIsStatic(const bool value)
{
    if (isStatic == value)
        return;

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
    if (value)
    {
        enableAllBehaviours();
    }
    else
    {
        disableAllBehaviours();
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
    for (const auto& comp : getComponents())
    {
        if (dynamic_cast<Transform*>(comp.get())) continue;

        if (ComponentType type = comp->getComponentType(); ComponentFactory::instance().isRegistered(type))
        {
            count++;
        }
    }
    archive.process(count);

    for (const auto& comp : getComponents())
    {
        if (dynamic_cast<Transform*>(comp.get())) continue;

        ComponentType type = comp->getComponentType();
        if (!ComponentFactory::instance().isRegistered(type))
        {
            continue;
        }

        auto typeId = static_cast<uint32_t>(type);
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
        const auto type = static_cast<ComponentType>(typeId);

        // Try to find existing component first

        if (Component* existing = getComponentByType(type))
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
            internalAddComponent(std::move(comp));
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
    for (const auto& comp : getComponents())
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


void GameObject::enableAllBehaviours() const
{
    for (auto& behaviour : behaviours)
    {
        behaviour->setEnabled(true);
    }
}

void GameObject::disableAllBehaviours() const
{
    for (auto& behaviour : behaviours)
    {
        behaviour->setEnabled(false);
    }
}

void GameObject::destroyAllComponents()
{
    while (!components.empty())
    {
        removeComponent(components.back().get());
    }
}

const std::vector<std::unique_ptr<Component>>& GameObject::getComponents() const
{
    return components;
}

void GameObject::internalAddComponent(std::unique_ptr<Component> component)
{
    if (component != nullptr)
    {
        components.push_back(std::move(component));
    }
}
