#pragma once
#ifndef INPUT_ADAPTER_H
#define INPUT_ADAPTER_H
class InputManager;

class IInputAdapter
{
  public:
    virtual ~IInputAdapter() = default;
    virtual void poll(InputManager &input) = 0;
};

#endif // INPUT_ADAPTER_H
