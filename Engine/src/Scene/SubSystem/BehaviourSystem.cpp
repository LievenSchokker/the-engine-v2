#include "Scene/SubSystems/BehaviourSystem.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "Core/ApplicationClock.h"

void BehaviourSystem::initialiseScene(Scene& scene, GameWorld& world)
{
    std::vector<Behaviour*> allBehaviours;
    scene.onStart(world);

    scene.forEachGameObject([&](GameObject& gameObject) {
        for (auto& behaviour  : gameObject.getAllBehaviours())
        {
            if (behaviour != nullptr)
            {
                allBehaviours.push_back(behaviour);
            }
        }
    });

    awakeBehaviours(allBehaviours, world);
    enableBehaviours(allBehaviours);
    startBehaviours(allBehaviours);
}

void BehaviourSystem::update(Scene& scene, double deltaTime, const GameWorld& world, bool fixed)
{
    const bool clockPaused = (world.clock != nullptr && world.clock->isPaused());



    std::vector<GameObject*> activeObjects;

    scene.forEachGameObject([&](GameObject& gameObject) {
        if (gameObject.getIsActive()) {
            activeObjects.push_back(&gameObject);
        }
    });

    for (GameObject* gameObject : activeObjects)
    {
        if (!gameObject)
            continue;

        for (Behaviour* behaviour : gameObject->getEnabledBehaviours())
        {
            if (!behaviour)
                continue;

            if (!behaviour->getHasAwakened() || !behaviour->getHasStarted())
                continue;

            if (clockPaused && !behaviour->shouldRunWhenPaused())
                continue;
			if (fixed)
			{
				behaviour->fixedUpdate(deltaTime);
			}
        	else
        	{
        		behaviour->update(deltaTime, world);
        	}
        }
    }
}


void BehaviourSystem::awakeBehaviours(const std::vector<Behaviour*>& behaviours, GameWorld& world)
{
    for (Behaviour* behaviour : behaviours)
    {
        if (!behaviour)
            continue;

        if (!behaviour->getHasAwakened())
            behaviour->awake(world);
    }
}

void BehaviourSystem::enableBehaviours(const std::vector<Behaviour*>& behaviours)
{
    for (Behaviour* behaviour : behaviours)
    {
        if (!behaviour)
            continue;

        const GameObject* owner = behaviour->getGameObject();
        if (!owner)
            continue;

        if (owner->getIsActive() && behaviour->getIsEnabled())
            behaviour->onEnable();
    }
}

void BehaviourSystem::startBehaviours(const std::vector<Behaviour*>& behaviours)
{
    for (Behaviour* behaviour : behaviours)
    {
        if (!behaviour)
            continue;

        if (!behaviour->getIsActiveAndEnabled())
            continue;

        if (!behaviour->getHasStarted())
            behaviour->start();
    }
}

void BehaviourSystem::disableBehaviours(const std::vector<Behaviour*>& behaviours)
{
    for (Behaviour* behaviour : behaviours)
    {
        if (!behaviour)
            continue;

        if (behaviour->getIsEnabled())
            behaviour->onDisable();
    }
}

void BehaviourSystem::initialiseRuntimeBehaviours(const std::vector<Behaviour*>& behaviours, GameWorld& world)
{
    awakeBehaviours(behaviours, world);
    enableBehaviours(behaviours);
    startBehaviours(behaviours);
}