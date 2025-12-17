#pragma once


#include "EngineLoop.h"

template <typename T>
T* EngineLoop::getSystem()
{
    for (auto& sys : systems)
    {
        if (auto* casted = dynamic_cast<T*>(sys.get()))
        {
            return casted;
        }
    }
    return nullptr;
}


