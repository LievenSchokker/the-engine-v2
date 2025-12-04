//
// Created by samle on 04/12/2025.
//


#include "External/SdlContext.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"
#include "Component/Transform.h"
#include "Component/ShapeRenderer.h"
#include "GameObject/GameObject.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue.h"
#include "Input/InputManager.h"

#include <memory>
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char* argv[])
{
    // --- SDL Setup ---
    SdlContext context(SDL_INIT_VIDEO);
    SDLRenderer renderer(context);

    WindowOptions options{"Simple Scene Example", SCREEN_WIDTH, SCREEN_HEIGHT};
    renderer.open(options);
    if (!renderer.isOpen()) {
        std::cerr << "Failed to open SDL window\n";
        return 1;
    }

    // --- Scene Manager ---
    SceneManager sceneManager;
    sceneManager.setClearColor(Color::darkGray());

    auto mainScene = std::make_unique<Scene>("MainScene");

    // --- Create GameObject ---
    auto box = std::make_unique<GameObject>();
    box->setName("BlueBox");

    // Transform component
    box->getTransform()->setPosition({200.0f, 150.0f});
    box->getTransform()->setScale({1.0f, 1.0f});

    // ShapeRenderer component
    auto* boxRenderer = box->addComponent<ShapeRenderer>();
    boxRenderer->setRectangle({100.0f, 100.0f}).setColor(Color::lightBlue());

    // Add GameObject to the scene
    mainScene->addGameObject(std::move(box));

    // Add scene to SceneManager and set active
    sceneManager.addScene(std::move(mainScene));
    sceneManager.setActiveScene("MainScene");

    // --- Input ---
    InputManager* input = InputManager::getInstance();

    RenderQueue renderQueue;
    Uint32 lastTicks = SDL_GetTicks();
    bool running = true;

    // --- Game loop ---
    while (running && renderer.isOpen()) {
        input->update();

        if (input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE))
            running = false;

        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        // --- Update ---
        sceneManager.update(deltaTime);

        // --- Render ---
        sceneManager.buildRenderQueue(renderQueue);
        executeRenderQueue(renderer, renderQueue);

        SDL_Delay(16); // ~60 FPS
    }

    renderer.close();
    return 0;
}
