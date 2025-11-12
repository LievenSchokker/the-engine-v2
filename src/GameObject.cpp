#include "../include/GameObject.h"

#include <utility>

GameObject::GameObject(std::string name) : name(std::move(name))
{
}
