//
// Created by samle on 14/12/2025.
//

#pragma once
#include "../IZandbak.h"

class UserInterfaceZandbak : public IZandbak
{
public:
	UserInterfaceZandbak() = default;
	~UserInterfaceZandbak() override = default;

	std::unique_ptr<Scene> getScene() override;
};
