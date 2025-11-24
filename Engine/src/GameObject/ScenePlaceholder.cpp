

#include "GameObject/ScenePlaceholder.h"
#include "GameObject/GameObject.h"
#include "Component/Component.h"

ScenePlaceholder::~ScenePlaceholder()
{
    for (auto* obj : objects)
        delete obj;

    objects.clear();
    destroyQueue.clear();
}

void ScenePlaceholder::addObject(GameObject* obj)
{
    objects.push_back(obj);
}


void ScenePlaceholder::queueDestroy(GameObject* obj)
{
    for (auto* queued : destroyQueue)
    {
        if (queued == obj)
            return;
    }

    destroyQueue.push_back(obj);
}


void ScenePlaceholder::processDestroyQueue()
{
    for (auto* obj : destroyQueue)
    {
        objects.erase(
            std::remove(objects.begin(), objects.end(), obj),
            objects.end()
        );

        obj->onSceneDestroy();
        delete obj;
    }

    destroyQueue.clear();
}