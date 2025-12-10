#pragma once

class SceneManager;
struct Vector2;

#include "NetworkContext.h"

<<<<<<< HEAD

class ServerNetworkContext: public NetworkContext
{
public:
	explicit ServerNetworkContext(SceneManager& sceneManager) : NetworkContext(
		sceneManager)
=======
class ServerNetworkContext: public NetworkContext
{
   public:
	explicit ServerNetworkContext(SceneManager& sceneManager)
		: NetworkContext(sceneManager)
>>>>>>> origin/development
	{
	}

	~ServerNetworkContext() override;
<<<<<<< HEAD
};
=======
};
>>>>>>> origin/development
