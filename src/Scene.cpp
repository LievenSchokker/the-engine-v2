#include "../include/Scene.h"

#include <algorithm>
#include <utility>

Scene::Scene(std::string name) : name(std::move(name)) {
}

const std::string& Scene::getName() const {
    return name;
}

bool Scene::addGameObject(std::unique_ptr<GameObject> gameObject) {
    if (!gameObject) {
        return false;
    }

    auto* rawPtr = gameObject.get();
    gameObjects.emplace_back(std::move(gameObject));

    if (active) {
        rawPtr->onStart();
    }

    return true;
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

std::unique_ptr<GameObject> Scene::extractGameObject(const std::string& name) {
    auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                           [&](const std::unique_ptr<GameObject>& gameObject) {
                               return gameObject->getName() == name;
                           });

    if (it == gameObjects.end()) {
        return nullptr;
    }

    // Call onStop if scene is active
    if (active) {
        (*it)->onStop();
    }

    // Move ownership and remove from vector
    auto result = std::move(*it);
    gameObjects.erase(it);
    return result;
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
