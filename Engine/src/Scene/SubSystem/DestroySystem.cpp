#include "Scene/SubSystems/DestroySystem.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"

#include <algorithm>

void DestroySystem::queueDestroy(GameObject* obj)
{
	if (obj == nullptr)
		return;

	if(obj->getIsDestroyed()){
		return;
	}

	destroyQueue.push_back(obj);
	obj->isDestroyed = true;
}

void DestroySystem::processQueue(Scene& scene)
{
	if (destroyQueue.empty())
		return;

	while(!destroyQueue.empty())
	{
		GameObject* obj = destroyQueue.back();
		destroyQueue.pop_back();

		if (obj == nullptr)
			continue;

		// Invoke callback if set
		if (preDestroyCallback)
			preDestroyCallback(obj);

		scene.removeGameObject(obj->getGameObjectHandle());
	}
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