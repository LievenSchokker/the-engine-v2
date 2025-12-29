//
// Created by samle on 04/12/2025.
#pragma once


#include "../IZandbak.h"
#include "Scene/Scene.h"

#include <memory>

class TransformZandbak final : public IZandbak
{
public:
	TransformZandbak() = default;
	~TransformZandbak() override = default;

	std::unique_ptr<Scene> getScene() override;
};