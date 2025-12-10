#pragma once

#include "NetworkContext.h"

class ClientNetworkContext: public NetworkContext
{
<<<<<<< HEAD
public:
	explicit ClientNetworkContext(SceneManager& sceneManager) : NetworkContext(
		sceneManager)
=======
   public:
	explicit ClientNetworkContext(SceneManager& sceneManager)
		: NetworkContext(sceneManager)
>>>>>>> origin/development
	{
	}

	~ClientNetworkContext() override;
<<<<<<< HEAD
};
=======
};
>>>>>>> origin/development
