#pragma once


class SceneManager;
class ISceneStrategy;
struct Vector2;

/// NOTE: This struct can be used to inject Application dependencies to the MessageHandlers.
<<<<<<< HEAD
/// Add more if neeced
struct NetworkContext
=======
/// Add more if needed
struct  NetworkContext
>>>>>>> origin/development
{
public:
	explicit NetworkContext(SceneManager& scenesManager) : sceneManager(
		&scenesManager)
	{
	}

	virtual ~NetworkContext() = default;

	SceneManager& getSceneManager() const
	{
		return *sceneManager;
	}

protected:
	SceneManager* sceneManager;
};