#include "Scene/SubSystems/DestroySystem.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"

#include <algorithm>

void DestroySystem::queueDestroy(GameObject* obj)
{
	if (obj == nullptr)
		return;

	if (isQueued(obj))
		return;

	destroyQueue.push_back(obj);
}

void DestroySystem::queueDestroy(const std::vector<GameObject*>& objects)
{
	for (GameObject* obj : objects)
	{
		queueDestroy(obj);
	}
}

void DestroySystem::processQueue(Scene& scene)
{
	if (destroyQueue.empty())
		return;

	// We iterate by index because the queue could grow
	// if onSceneDestroy() queues more objects
    // DON'T MAKE THIS A NESTED LOOP
	for (size_t i = 0; i < destroyQueue.size(); ++i)
	{
		GameObject* obj = destroyQueue[i];

		if (obj == nullptr)
			continue;

		// Invoke callback if set
		if (preDestroyCallback)
			preDestroyCallback(obj);

		obj->onSceneDestroy();
		scene.removeGameObject(obj);
	}

	destroyQueue.clear();
}

bool DestroySystem::isQueued(const GameObject* obj) const
{
	if (obj == nullptr)
		return false;

	return std::ranges::find(destroyQueue, obj)
		   != destroyQueue.end();
}

size_t DestroySystem::queueSize() const
{
	return destroyQueue.size();
}

void DestroySystem::clear()
{
	destroyQueue.clear();
}

void DestroySystem::setPreDestroyCallback(std::function<void(GameObject*)> callback)
{
	preDestroyCallback = std::move(callback);
}