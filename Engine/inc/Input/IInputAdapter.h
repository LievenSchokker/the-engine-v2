#pragma once


class InputManager;

class IInputAdapter
{
  public:
    virtual ~IInputAdapter() = default;
    virtual void poll(InputManager& input) = 0;
};

