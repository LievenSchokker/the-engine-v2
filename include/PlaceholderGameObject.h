#pragma once

#include "GameObject.h"

class PlaceholderGameObject : public GameObject
{
  public:
    explicit PlaceholderGameObject(std::string name);

    void onStart() override;
    void onStop() override;
    void update(float deltaTime) override;
    void render() override;

  private:
    float accumulatedTime = 0.0f;
};
