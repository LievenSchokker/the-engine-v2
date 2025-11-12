#include "../include/SceneManager.h"

#include <utility>

Scene* SceneManager::addScene(std::unique_ptr<Scene> scene)
{
    if (!scene)
    {
        return nullptr;
    }

    const std::string name = scene->getName();
    if (scenes.contains(name))
    {
        return scenes[name].get();
    }

    auto* rawPtr = scene.get();
    scenes.emplace(name, std::move(scene));
    return rawPtr;
}

bool SceneManager::removeScene(const std::string& name)
{
    const auto it = scenes.find(name);
    if (it == scenes.end())
    {
        return false;
    }

    if (it->second.get() == activeScene)
    {
        activeScene->onStop();
        activeScene = nullptr;
        paused = false;
    }

    scenes.erase(it);
    return true;
}

Scene* SceneManager::getScene(const std::string& name) const
{
    const auto it = scenes.find(name);
    if (it != scenes.end())
    {
        return it->second.get();
    }

    return nullptr;
}

bool SceneManager::setActiveScene(const std::string& name)
{
    if (activeScene && activeScene->getName() == name)
    {
        return true;
    }

    Scene* nextScene = getScene(name);
    if (!nextScene)
    {
        return false;
    }

    if (activeScene)
    {
        activeScene->onStop();
    }

    activeScene = nextScene;
    paused = false;
    activeScene->onStart();
    return true;
}

bool SceneManager::loadScene(const std::string& name)
{
    return setActiveScene(name);
}

void SceneManager::pause()
{
    if (!activeScene || paused)
    {
        return;
    }

    paused = true;
    activeScene->onStop();
}

void SceneManager::resume()
{
    if (!activeScene || !paused)
    {
        return;
    }

    paused = false;
    activeScene->onStart();
}

void SceneManager::update(float deltaTime)
{
    if (activeScene && !paused)
    {
        activeScene->update(deltaTime);
    }
}

void SceneManager::render()
{
    if (activeScene && !paused)
    {
        activeScene->render();
    }
}
