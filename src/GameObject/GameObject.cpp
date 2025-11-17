#include "../../inc/GameObject/GameObject.h"

#include <utility>

GameObject::GameObject(std::string name) : name(std::move(name)) {
}

const std::string& GameObject::getName() const {
    return name;
}