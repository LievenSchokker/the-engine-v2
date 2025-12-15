//
// Created by samle on 04/12/2025.
//

#pragma once

#include "Scene/Scene.h"
#include <memory>



class IZandbak
{
public:
    IZandbak() = default;
    virtual ~IZandbak() = default;

    virtual std::unique_ptr<Scene> getScene() = 0;
};
