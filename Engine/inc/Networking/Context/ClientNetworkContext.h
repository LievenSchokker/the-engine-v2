//
// Created by samle on 25/11/2025.
//


#pragma once


#include "NetworkContext.h"

class ClientNetworkContext: public NetworkContext
{
public:
	explicit ClientNetworkContext(SceneManager& sceneManager) : NetworkContext(
		sceneManager)
	{
	}

	~ClientNetworkContext() override;
};