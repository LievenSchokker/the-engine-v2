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
            spawnNewObject(gameWorld);
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

        std::cout << "\n>>> SPAWNING: " << objectName << " <<<" << std::endl;

        auto newObject = std::make_unique<GameObject>();
        newObject->setName(objectName);
        newObject->setTag("Spawned");

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
        newObject->addComponent<TestLifecycleBehaviour>(objectName);
        spawnedNames.push_back(objectName);
        Scene* scene = gameWorld.sceneManager->getActiveScene();
        if (scene)
        {
            scene->addGameObject(std::move(newObject));
            std::cout << ">>> SPAWN COMPLETE: " << objectName
                      << " (Total spawned: " << spawnedNames.size() << ") <<<\n" << std::endl;
        }
        else
        {
            std::cerr << "[SpawnDestroyInput] ERROR: No active scene!" << std::endl;
            spawnedNames.pop_back();
        }
    }

    void destroyAllSpawned(const GameWorld& gameWorld)
    {
        if (spawnedNames.empty())
        {
            std::cout << "\n>>> No spawned objects to destroy <<<\n" << std::endl;
            return;
        }

        std::cout << "\n>>> DESTROYING ALL " << spawnedNames.size() << " SPAWNED OBJECTS <<<" << std::endl;

        Scene* scene = gameWorld.sceneManager->getActiveScene();
        if (!scene)
        {
            std::cerr << "[SpawnDestroyInput] ERROR: No active scene!" << std::endl;
            return;
        }

        int destroyedCount = 0;
        for (const std::string& name : spawnedNames)
        {
            GameObject* obj = scene->getGameObject(name);
            if (obj)
            {
                std::cout << "  Destroying: " << name << std::endl;
                obj->destroy();
                destroyedCount++;
            }
            else
            {
                std::cout << "  [Already destroyed]: " << name << std::endl;
            }
        }

        spawnedNames.clear();

        std::cout << ">>> DESTROY COMPLETE: " << destroyedCount << " objects destroyed <<<\n" << std::endl;
    }

    int spawnCount;
    std::vector<std::string> spawnedNames;
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