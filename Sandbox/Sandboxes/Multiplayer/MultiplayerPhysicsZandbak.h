#pragma once


#include "Scene/Scene.h"
#include "IZandbak.h"

#include <memory>

class MultiplayerPhysicsZandbak final : public IZandbak
{
public:
	MultiplayerPhysicsZandbak() = default;
	~MultiplayerPhysicsZandbak() override = default;

	std::unique_ptr<Scene> getScene() override;

};
