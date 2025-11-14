#include "../include/SceneManager.h"

#include <iostream>
#include <utility>

Scene* SceneManager::addScene(std::unique_ptr<Scene> scene) {
    if (!scene) {
        std::cerr << "[SceneManager] Error: Attempted to add a null scene\n";
        return nullptr;
    }

    const std::string name = scene->getName();
    if (scenes.contains(name)) {
        std::cerr << "[SceneManager] Error: Scene with name '" << name << "' already exists\n";
        return nullptr;
    }

    auto* rawPtr = scene.get();
    scenes.emplace(name, std::move(scene));
    return rawPtr;
}

bool SceneManager::removeScene(const std::string& name) {
    const auto it = scenes.find(name);
    if (it == scenes.end()) {
        return false;
    }

    if (it->second.get() == activeScene) {
        activeScene->onStop();
        activeScene = nullptr;
        paused = false;
    } else {
        it->second->onStop();
    }

    scenes.erase(it);
    return true;
}

Scene* SceneManager::getScene(const std::string& name) const {
    const auto it = scenes.find(name);
    if (it != scenes.end()) {
        return it->second.get();
    }

    return nullptr;
}

bool SceneManager::transferGameObject(const std::string& fromSceneName,
                                      const std::string& toSceneName,
                                      const std::string& objectName) {
    Scene* fromScene = getScene(fromSceneName);
    Scene* toScene = getScene(toSceneName);

    if (!fromScene || !toScene) {
        std::cerr << "[SceneManager] Error: Scene not found\n";
        return false;
    }

    // Check if object exists in source scene
    if (!fromScene->getGameObject(objectName)) {
        std::cerr << "[SceneManager] Error: GameObject '" << objectName << "' not found in scene '"
                  << fromSceneName << "'\n";
        return false;
    }

    // Check if object already exists in target scene
    if (toScene->getGameObject(objectName)) {
        std::cerr << "[SceneManager] Error: GameObject '" << objectName
                  << "' already exists in scene '" << toSceneName << "'\n";
        return false;
    }

    // Extract and transfer
    auto gameObject = fromScene->extractGameObject(objectName);
    if (!gameObject) {
        return false;
    }

    toScene->addGameObject(std::move(gameObject));
    return true;
}

bool SceneManager::setActiveScene(const std::string& name) {
    if (activeScene && activeScene->getName() == name) {
        std::cout << "[SceneManager] Warning: Scene with name '" << name << "' is already active\n";
        return true;
    }

    Scene* nextScene = getScene(name);
    if (!nextScene) {
        std::cerr << "[SceneManager] Error: Scene with name '" << name << "' not found\n";
        return false;
    }

    if (activeScene) {
        activeScene->onStop();
    }

    activeScene = nextScene;
    paused = false;
    activeScene->onStart();
    return true;
}

bool SceneManager::loadScene(const std::string& name) {
    return setActiveScene(name);
}

void SceneManager::pause() {
    if (!activeScene || paused) {
        return;
    }

    paused = true;
    activeScene->onPause();
}

void SceneManager::resume() {
    if (!activeScene || !paused) {
        return;
    }

    paused = false;
    activeScene->onResume();
}

void SceneManager::update(float deltaTime) {
    if (activeScene && !paused) {
        activeScene->update(deltaTime);
    }
}

void SceneManager::render() {
    if (activeScene && !paused) {
        activeScene->render();
    }
}
