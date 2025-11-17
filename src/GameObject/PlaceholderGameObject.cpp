#include "../../inc/GameObject/PlaceholderGameObject.h"

#include <iostream>
#include <utility>

PlaceholderGameObject::PlaceholderGameObject(std::string name) : GameObject(std::move(name)) {
}

void PlaceholderGameObject::onStart() {
    std::cout << "[PlaceholderGameObject] " << getName() << " started\n";
    accumulatedTime = 0.0f;
}

void PlaceholderGameObject::onStop() {
    std::cout << "[PlaceholderGameObject] " << getName() << " stopped\n";
}

void PlaceholderGameObject::onPause() {
    std::cout << "[PlaceholderGameObject] " << getName() << " paused\n";
}

void PlaceholderGameObject::onResume() {
    std::cout << "[PlaceholderGameObject] " << getName() << " resumed\n";
}

void PlaceholderGameObject::update(float deltaTime) {
    accumulatedTime += deltaTime;
    std::cout << "[PlaceholderGameObject] " << getName()
              << " updated, accumulated time: " << accumulatedTime << "\n";
}
