#pragma once

#include "GameObject.h"

#include <memory>
#include <string>
#include <vector>

class Scene
{
  public:
    explicit Scene(std::string name);

    const std::string& getName() const
    {
        return name;
    }

    GameObject* addGameObject(std::unique_ptr<GameObject> gameObject);
    bool removeGameObject(const std::string& name);
    GameObject* getGameObject(const std::string& name) const;

    void onStart();
    void onStop();
    void update(float deltaTime);
    void render();

  private:
    std::string name;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    bool active = false;
};
