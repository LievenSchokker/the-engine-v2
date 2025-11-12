#pragma once

#include "Scene.h"

#include <memory>
#include <string>
#include <unordered_map>

class SceneManager
{
  public:
    SceneManager() = default;

    Scene* addScene(std::unique_ptr<Scene> scene);
    bool removeScene(const std::string& name);

    Scene* getScene(const std::string& name) const;

    Scene* getActiveScene() const
    {
        return activeScene;
    }

    bool setActiveScene(const std::string& name);
    bool loadScene(const std::string& name);

    void pause();
    void resume();

    bool isPaused() const
    {
        return paused;
    }

    void update(float deltaTime);
    void render();

  private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* activeScene = nullptr;
    bool paused = false;
};
