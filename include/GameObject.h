#pragma once

#include <string>

class GameObject
{
  public:
    explicit GameObject(std::string name);
    virtual ~GameObject() = default;

    const std::string& getName() const
    {
        return name;
    }

    virtual void onStart()
    {
    }

    virtual void onStop()
    {
    }

    virtual void update(float /*deltaTime*/)
    {
    }

    virtual void render()
    {
    }

  private:
    std::string name;
};
