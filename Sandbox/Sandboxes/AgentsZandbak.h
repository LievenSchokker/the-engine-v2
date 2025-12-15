//
// Created by samle on 04/12/2025.
#pragma once


#include "../IZandbak.h"
#include "Scene/Scene.h"

#include <memory>

class AgentsZandbak final : public IZandbak
{
public:
    AgentsZandbak() = default;
    ~AgentsZandbak() override = default;

    std::unique_ptr<Scene> getScene() override;

};