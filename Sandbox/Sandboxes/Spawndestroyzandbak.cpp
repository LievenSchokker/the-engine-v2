#include "SpawnDestroyZandbak.h"
#include "Behaviour/Behaviour.h"
#include "Behaviours/TestLifeCycleBehaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/GameWorld.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <random>
#include <string>
#include <vector>

/**
 * @class SpawnDestroyInputBehaviour
 * @brief Handles L/K input for spawning and destroying test objects.
 */
class SpawnDestroyInputBehaviour : public Behaviour
{
public:
    SpawnDestroyInputBehaviour() : spawnCount(0)
    {
    }

    ~SpawnDestroyInputBehaviour() override = default;

    void onAwake() override
    {
        std::cout << "\n========================================" << std::endl;
        std::cout << "  SpawnDestroy Test Scene" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "  L - Spawn new GameObject with behaviour" << std::endl;
        std::cout << "  K - Destroy all spawned GameObjects" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    void onEnable() override
    {
    }

    void onStart() override
    {
        std::cout << "[SpawnDestroyInput] Ready for input" << std::endl;
    }

    void update(double deltaTime, const GameWorld& gameWorld) override
    {
        (void)deltaTime;
    	
        if (gameWorld.input->wasKeyPressed(KeyCode::L))
        {
			for(int i = 0; i < 10000; i++){
				spawnNewObject(gameWorld);
			}
        }

        if (gameWorld.input->wasKeyPressed(KeyCode::K))
        {
            destroyAllSpawned(gameWorld);
        }
    }

private:
    void spawnNewObject(const GameWorld& gameWorld)
    {
        spawnCount++;
        std::string objectName = "SpawnedObject_" + std::to_string(spawnCount);

        auto newObject = std::make_unique<GameObject>();
        newObject->setName(objectName);

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> xDist(100.0f, 600.0f);
        std::uniform_real_distribution<float> yDist(100.0f, 600.0f);

        newObject->getTransform()->setPosition({xDist(gen), yDist(gen)});
        auto* renderer = newObject->addComponent<ShapeRenderer>();
        renderer->setCircle(25.0f);

        Color colors[] = {
            Color::lightBlue(),
            Color::lightRed(),
            Color::green(),
            Color::yellow(),
            Color::darkPurple(),
            Color(255, 128, 0)
        };

        renderer->setColor(colors[spawnCount % 6]);

        Scene* scene = gameWorld.sceneManager->getActiveScene();
        if (scene)
        {
			spawnedObjectHandles.push_back(scene->addGameObject(std::move(newObject)));
        }
        else
        {
			spawnedObjectHandles.pop_back();
        }
    }

    void destroyAllSpawned(const GameWorld& gameWorld)
    {
        if ( spawnedObjectHandles.empty())
        {
            return;
        }

        Scene* scene = gameWorld.sceneManager->getActiveScene();
        if (!scene)
        {
            return;
        }

        int destroyedCount = 0;
        for (const ObjectHandle& handle : spawnedObjectHandles )
        {
			scene->removeGameObject(handle);
        }

		spawnedObjectHandles.clear();
    }

    int spawnCount;
    std::vector<ObjectHandle> spawnedObjectHandles;
};

std::unique_ptr<Scene> SpawnDestroyZandbak::getScene()
{
    auto scene = std::make_unique<Scene>("SpawnDestroyTestScene");
    auto inputHandler = std::make_unique<GameObject>();
    inputHandler->setName("InputHandler");
    inputHandler->addComponent<SpawnDestroyInputBehaviour>();
    scene->addGameObject(std::move(inputHandler));
    auto referenceObject = std::make_unique<GameObject>();
    referenceObject->setName("ReferenceObject");
    referenceObject->getTransform()->setPosition({350.0f, 350.0f});
    auto* refRenderer = referenceObject->addComponent<ShapeRenderer>();
    refRenderer->setRectangle({50.0f, 50.0f});
    refRenderer->setColor(Color::white());
    scene->addGameObject(std::move(referenceObject));

    return scene;
}