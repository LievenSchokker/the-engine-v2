#pragma once

class SceneManager;
struct Vector2;

#include "NetworkContext.h"

class ServerNetworkContext: public NetworkContext
{
   public:
	explicit ServerNetworkContext(SceneManager& sceneManager)
		: NetworkContext(sceneManager)
	{
	}

	~ServerNetworkContext() override;
};
