#include "../include/Scene.h"

#include <algorithm>
#include <utility>

Scene::Scene(std::string name) : name(std::move(name)) {
}

const std::string& Scene::getName() const {
    return name;
}

GameObject* Scene::addGameObject(std::unique_ptr<GameObject> gameObject) {
    if (!gameObject) {
        return nullptr;
    }

    auto* rawPtr = gameObject.get();
    gameObjects.emplace_back(std::move(gameObject));

    if (active) {
        rawPtr->onStart();
    }

    return rawPtr;
}

bool Scene::removeGameObject(const std::string& name) {
    const auto it = std::remove_if(gameObjects.begin(), gameObjects.end(),
                                   [&](const std::unique_ptr<GameObject>& gameObject) {
                                       if (gameObject->getName() == name) {
                                           if (active)  // TODO: After the gameobject & component
                                                        // PR, also check here for the gameobject's
                                                        // component's active state
                                           {
                                               gameObject->onStop();
                                           }
                                           return true;
                                       }
                                       return false;
                                   });

    if (it != gameObjects.end()) {
        gameObjects.erase(it, gameObjects.end());
        return true;
    }

    return false;
}

GameObject* Scene::getGameObject(const std::string& name) const {
    for (const auto& gameObject : gameObjects) {
        if (gameObject->getName() == name) {
            return gameObject.get();
        }
    }

    return nullptr;
}

void Scene::onStart() {
    if (active) {
        return;
    }

    active = true;
    for (auto& gameObject : gameObjects) {
        gameObject->onStart();
    }
}

void Scene::onStop() {
    if (!active) {
        return;
    }

    active = false;
    for (auto& gameObject : gameObjects) {
        gameObject->onStop();
    }
}

void Scene::onPause() {
    if (!active) {
        return;
    }

    for (auto& gameObject : gameObjects) {
        gameObject->onPause();
    }
}

void Scene::onResume() {
    if (!active) {
        return;
    }

    for (auto& gameObject : gameObjects) {
        gameObject->onResume();
    }
}

void Scene::update(float deltaTime) {
    if (!active) {
        return;
    }

    for (auto& gameObject : gameObjects) {
        gameObject->update(deltaTime);
    }
}

void Scene::render() {
    if (!active) {
        return;
    }

    for (auto& gameObject : gameObjects) {
        gameObject->render();
    }
}
