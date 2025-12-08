#pragma once


class GameObject;

#include <vector>

class ScenePlaceholder
{
public:
	ScenePlaceholder() = default;
	~ScenePlaceholder();

	void addObject(GameObject* obj);

	void queueDestroy(GameObject* obj);

	void processDestroyQueue();

private:
	std::vector<GameObject*> objects;
	std::vector<GameObject*> destroyQueue;
};