#pragma once


#include "MultiplayerZandbak.h"
#include "Scene/Scene.h"
#include "IZandbak.h"

#include <memory>

class MultiplayerZandbak final : public IZandbak
{
public:
    MultiplayerZandbak() = default;
    ~MultiplayerZandbak() override = default;

    std::unique_ptr<Scene> getScene() override;

};
