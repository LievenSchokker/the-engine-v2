#include <iostream>
#include <random>

#include "EntryPoint.h"
#include "Behaviour/Behaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Component/Profiler/Profiler.h"
#include "Component/UIObject/Button.h"
#include "Component/UIObject/UIPanelObject.h"
#include "Events/EventQueue.h"
#include "../Engine/inc/Events/EventImplementations/UserInterfaceEvent.h"
#include "Physics/IPhysicsWorld.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "Rendering/Color.h"
#include "Scene/SceneManager.h"

#undef main

/**
 * @brief Spawns physics objects when a button is clicked
 */
class ObjectSpawnerBehaviour : public Behaviour
{
public:
    uint32_t spawnButtonId = 0;
    uint32_t clearButtonId = 0;

    void onAwake() override
    {
        std::random_device rd;
        rng.seed(rd());
    }

	void onStart() override
    {
    	subscribe<UIButtonClickedEvent>(&ObjectSpawnerBehaviour::onButtonClicked);
    }

	void update(float deltaTime, GameWorld* world) override
    {
    }

private:
    std::mt19937 rng;
    std::vector<std::string> spawnedObjectNames;
    int spawnCounter = 0;

    // Event handler - must take const EventType& parameter
    void onButtonClicked(const UIButtonClickedEvent& event)
    {
        std::cout << "Button clicked! id=" << event.buttonId << std::endl;

        if (event.buttonId == spawnButtonId)
        {
            spawnRandomObjects(10);
        }
        else if (event.buttonId == clearButtonId)
        {
            clearAllSpawned();
        }
    }

    void spawnRandomObjects(int count)
    {
        GameWorld* world = getWorld();
        if (!world || !world->sceneManager) return;

        Scene* scene = world->sceneManager->getActiveScene();
        if (!scene) return;

        std::uniform_real_distribution<float> xDist(100.0f, 600.0f);
        std::uniform_real_distribution<float> yDist(50.0f, 200.0f);
        std::uniform_real_distribution<float> sizeDist(10.0f, 30.0f);
        std::uniform_int_distribution<int> shapeDist(0, 1);
        std::uniform_int_distribution<int> colorDist(50, 255);

        for (int i = 0; i < count; ++i)
        {
            std::string objectName = "SpawnedObject_" + std::to_string(spawnCounter++);

            auto obj = std::make_unique<GameObject>();
            obj->setName(objectName);

            float x = xDist(rng);
            float y = yDist(rng);
            obj->getTransform()->setPosition({x, y});

            float size = sizeDist(rng);
            bool isCircle = shapeDist(rng) == 0;

            auto* rb = obj->addComponent<RigidBody>();
            rb->makeDynamic();

            auto* collider = obj->addComponent<Collider>();
            auto* shape = obj->addComponent<ShapeRenderer>();
            if (isCircle)
            {
                collider->setCircle(size);
                shape->setCircle(size);
            }
            else
            {
                collider->setRectangle({size * 2, size * 2});
                shape->setRectangle({size * 2, size * 2});
            }

            shape->setColor(Color(
                colorDist(rng),
                colorDist(rng),
                colorDist(rng),
                255
            ));

            if (world->physics)
            {
                world->physics->createBody(rb);
            }

            scene->addGameObject(std::move(obj));
            spawnedObjectNames.push_back(objectName);
        }

        std::cout << "Spawned " << count << " objects! Total: "
                  << spawnedObjectNames.size() << std::endl;
    }

    void clearAllSpawned()
    {
        GameWorld* world = getWorld();
        if (!world || !world->sceneManager) return;

        Scene* scene = world->sceneManager->getActiveScene();
        if (!scene) return;

        for (const std::string& name : spawnedObjectNames)
        {
            scene->removeGameObject(name);
        }

        std::cout << "Cleared " << spawnedObjectNames.size() << " objects!" << std::endl;
        spawnedObjectNames.clear();
    }
};

int main(int argc, char** argv)
{
    ApplicationSpecifications spec = {};
    spec.networkingOptions.port = 8080;
    spec.networkingOptions.serverIP = "127.0.0.1";
    spec.networkingOptions.mode = EngineMode::CLIENT;
    spec.networkingOptions.tickRate = 60;
    spec.renderBackend = RenderBackend::SDL;
    spec.windowOptions = {"Object Spawner Demo", 700, 700};

    std::unique_ptr<Game> game = std::make_unique<Game>();
    std::unique_ptr<Scene> scene = std::make_unique<Scene>("SpawnerScene");

    const uint32_t PANEL_ID = 10;

    // Panel
    auto panelObject = std::make_unique<GameObject>();
    auto* panel = panelObject->addComponent<UIPanelObject>(
        10.0f, 10.0f, 200.0f, 120.0f, "Spawner Controls"
    );
    panel->setBorder(true);
    panel->setColumns(1);
    panel->setRowHeight(35.0f);
    panel->setPanelId(PANEL_ID);
    scene->addGameObject(std::move(panelObject));

    // Spawn Button
    auto spawnButtonObject = std::make_unique<GameObject>();
    auto* spawnButton = spawnButtonObject->addComponent<Button>(
        0, 0, 180, 30, "Spawn Objects!"
    );
    spawnButton->setPanelId(PANEL_ID);
    scene->addGameObject(std::move(spawnButtonObject));

    // Clear Button
    auto clearButtonObject = std::make_unique<GameObject>();
    auto* clearButton = clearButtonObject->addComponent<Button>(
        0, 0, 180, 30, "Clear All"
    );
    clearButton->setPanelId(PANEL_ID);
    scene->addGameObject(std::move(clearButtonObject));

    // Spawner Behaviour
    auto spawnerObject = std::make_unique<GameObject>();
    auto* spawner = spawnerObject->addComponent<ObjectSpawnerBehaviour>();
    spawner->spawnButtonId = spawnButton->getButtonId();
    spawner->clearButtonId = clearButton->getButtonId();
    scene->addGameObject(std::move(spawnerObject));

    // Static floor
    auto floor = std::make_unique<GameObject>();
    floor->setName("Floor");
    floor->getTransform()->setPosition({350.0f, 650.0f});

    auto* floorRb = floor->addComponent<RigidBody>();
    floorRb->makeStatic();

    auto* floorCollider = floor->addComponent<Collider>();
    floorCollider->setRectangle({700.0f, 40.0f});

    auto* floorShape = floor->addComponent<ShapeRenderer>();
    floorShape->setRectangle({700.0f, 40.0f});
    floorShape->setColor(Color(100, 100, 100, 255));
	floor->addComponent<Profiler>(100, 100, 100, 100);
    scene->addGameObject(std::move(floor));

    game->addScene(std::move(scene));
    game->setApplicationSpecifications(spec);

    return SpelMotorEntry::main(std::move(game));
}