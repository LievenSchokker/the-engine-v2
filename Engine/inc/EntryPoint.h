#pragma once

#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"

#include <cstring>
#include <iostream>

#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Scene/SceneManager.h"
#include "Demo/PlayerMovement.h"
#include "Networking/NetworkPrefabRegistry.h"

#undef main

namespace SpelMotorEntry
{
    inline void registerNetworkPrefabs()
    {
        auto& registry = NetworkPrefabRegistry::instance();

        registry.registerPrefab(1, []() {
            auto player = std::make_unique<GameObject>();
            player->setName("NetworkPlayer");

            auto* shape = player->addComponent<ShapeRenderer>();
            shape->setCircle(20.0f).setColor(Color(51, 153, 255, 255));

            player->addComponent<PlayerMovement>();
            player->addComponent<NetworkIdentity>();

            return player;
        });

    }
    inline std::unique_ptr<Scene> createWalkingScene()
    {
        auto scene = std::make_unique<Scene>("WalkingDemo");

        // Just obstacles - players are spawned by NetworkSpawnManager
        auto addObstacle = [&](const std::string& name, Vector2 pos, Vector2 size, Color color) {
            auto obj = std::make_unique<GameObject>();
            obj->setName(name);
            obj->getTransform()->setPosition(pos);
            obj->addComponent<ShapeRenderer>()->setRectangle(size).setColor(color);
            scene->addGameObject(std::move(obj));
        };

        addObstacle("Box1", {150, 150}, {60, 60}, Color(153, 102, 51, 255));
        addObstacle("Box2", {500, 200}, {80, 40}, Color(102, 128, 77, 255));
        addObstacle("Box3", {200, 450}, {50, 80}, Color(128, 77, 102, 255));
        addObstacle("Box4", {550, 500}, {70, 70}, Color(77, 102, 128, 255));
        addObstacle("Box5", {100, 300}, {40, 100}, Color(179, 77, 77, 255));

        return scene;
    }

    inline int main(int argc, char** argv)
    {
        // Register prefabs FIRST
        registerNetworkPrefabs();

        ApplicationSpecifications specifications{};
        specifications.windowOptions = {"Walking Demo", 700, 700};
        specifications.renderBackend = RenderBackend::SDL;
        specifications.networkingOptions.tickRate = 60;
        specifications.networkingOptions.port = 7777;
        specifications.networkingOptions.serverIP = "127.0.0.1";
        specifications.networkingOptions.mode = EngineMode::CLIENT;

        if (argc > 1)
        {
            if (std::strcmp(argv[1], "server") == 0)
            {
                specifications.networkingOptions.mode = EngineMode::SERVER;
                std::cout << "Starting SERVER on port 7777\n";
            }
            else if (std::strcmp(argv[1], "client") == 0)
            {
                specifications.networkingOptions.mode = EngineMode::CLIENT;
                std::cout << "Starting CLIENT\n";
            }
        }

        SpelMotor engine(specifications);

        engine.getSceneManager()->addScene(createWalkingScene());
        engine.getSceneManager()->setActiveScene("WalkingDemo");

        engine.run();
        return 0;
    }
}
