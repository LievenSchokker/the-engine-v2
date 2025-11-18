#pragma once

#include <string>

class GameObject {
   public:
    explicit GameObject(std::string name);
    virtual ~GameObject() = default;

    const std::string& getName() const;

    virtual void onStart() = 0;

    virtual void onStop() = 0;

    virtual void onPause() = 0;

    virtual void onResume() = 0;

    // TODO: Replace this game object update for a behavior with an update
    virtual void update(float /*deltaTime*/) = 0;

   private:
    std::string name;
};
