/**
 * @file tilemap_example.cpp
 * @brief Example demonstrating how to use the Tilemap system
 *
 * This example shows:
 * - Loading a tilemap from a CSV file
 * - Creating a GameObject with TilemapComponent
 * - Setting tile colors
 * - Using grid query functions
 */

#include "../Engine/inc/Assets/AssetManager.h"
#include "../Engine/inc/Assets/TilemapAsset.h"
#include "../Engine/inc/Component/GridComponent.h"
#include "../Engine/inc/Component/ShapeRenderer.h"
#include "../Engine/inc/Component/TilemapComponent.h"
#include "../Engine/inc/Component/Transform.h"
#include "../Engine/inc/External/SdlContext.h"
#include "../Engine/inc/GameObject/GameObject.h"
#include "../Engine/inc/Input/InputManager.h"
#include "../Engine/inc/Input/KeyCode.h"
#include "../Engine/inc/Rendering/Color.h"
#include "../Engine/inc/Rendering/RenderQueue.h"
#include "../Engine/inc/Rendering/SDL/SDLRenderer.h"
#include "../Engine/inc/Rendering/Window/WindowOptions.h"
#include "../Engine/inc/Scene/SceneManager.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int main()
{
	SdlContext context(SDL_INIT_VIDEO);
	SDLRenderer renderer(context);

	WindowOptions options{"Tilemap Example", SCREEN_WIDTH, SCREEN_HEIGHT};
	renderer.open(options);
	if ( !renderer.isOpen() ) {
		std::cout << "Failed to open SDL window\n";
		return 1;
	}

	SceneManager sceneManager;
	sceneManager.setClearColor(Color::black());

	// Create a scene
	auto tilemapScene = std::make_unique<Scene>("TilemapScene");

	// Create AssetManager and load tilemap
	AssetManager assetManager;
	std::string tilemapPath = "Sandbox/Assets/level1_tilemap.csv";

	assetManager.add(tilemapPath, std::make_unique<TilemapAsset>());
	if ( !assetManager.load(tilemapPath) ) {
		std::cerr << "Failed to load tilemap: " << tilemapPath << std::endl;
		return 1;
	}

	TilemapAsset* tilemap =
		dynamic_cast<TilemapAsset*>(assetManager.get(tilemapPath));
	if ( tilemap == nullptr ) {
		std::cerr << "Failed to get tilemap asset" << std::endl;
		return 1;
	}

	// Create a GameObject for the tilemap
	auto tilemapObject = std::make_unique<GameObject>();
	tilemapObject->setName("LevelTilemap");

	// Add TilemapComponent
	auto* tilemapComponent = tilemapObject->addComponent<TilemapComponent>();
	if ( tilemapComponent == nullptr ) {
		std::cerr << "Failed to add TilemapComponent" << std::endl;
		return 1;
	}

	// Configure the tilemap component
	tilemapComponent->setTilemapAsset(tilemap);

	// Set tile size based on your assets and desired game scale.
	// Common sizes: 16x16, 32x32, 48x48, 64x64 pixels.
	// This is the recommended approach - developers should manually configure
	// tile size to match their art assets and game design.
	tilemapComponent->setTileSize({32.0, 32.0});

	// Set colors for different tile IDs
	tilemapComponent->setTileColor(0, Color::lightGreen());	 // Low grass
	tilemapComponent->setTileColor(1, Color::darkGray());	 // Rocks
	tilemapComponent->setTileColor(2, Color::green());		 // High grass
	tilemapComponent->setTileColor(3, Color::blue());		 // Water

	// Add GridComponent for AI pathfinding (future use)
	auto* gridComponent = tilemapObject->addComponent<GridComponent>();
	std::unique_ptr<GameObject> blockedBoulder;
	if ( gridComponent != nullptr ) {
		gridComponent->setTilemapComponent(tilemapComponent);
		gridComponent->setWalkableTileIds({0, 2});

		// Mark a sample blocked cell dynamically and visualize it with a
		// boulder
		Vector2 blockedCell{10.0, 7.0};
		gridComponent->blockCell(blockedCell);

		blockedBoulder = std::make_unique<GameObject>();
		blockedBoulder->setName("BlockedBoulder");
		auto* boulderRenderer = blockedBoulder->addComponent<ShapeRenderer>();
		boulderRenderer->setRectangle(tilemapComponent->getTileSize())
			.setColor(Color::gray());
		blockedBoulder->getTransform()->setPosition(
			tilemapComponent->cellToWorld(blockedCell));
	}

	// Add to scene
	tilemapScene->addGameObject(std::move(tilemapObject));
	if ( blockedBoulder != nullptr ) {
		// Append after the tilemap so it renders on top
		tilemapScene->addGameObject(std::move(blockedBoulder));
	}

	sceneManager.addScene(std::move(tilemapScene));
	sceneManager.setActiveScene("TilemapScene");

	// Example: Query grid information
	if ( tilemapComponent->isReady() ) {
		std::cout << "Tilemap loaded: " << tilemapComponent->getGridWidth()
				  << "x" << tilemapComponent->getGridHeight() << " tiles\n";

		// Test coordinate conversion
		Vector2 worldPos = {100.0, 100.0};
		Vector2 cell = tilemapComponent->worldToCell(worldPos);
		int tileId = tilemapComponent->getTileAt(cell);
		std::cout << "World position (" << worldPos.x << ", " << worldPos.y
				  << ") -> Cell (" << cell.x << ", " << cell.y
				  << ") -> Tile ID: " << tileId << "\n";
	}

	bool running = true;
	Uint32 lastTicks = SDL_GetTicks();
	RenderQueue renderQueue;

	InputManager* input = InputManager::getInstance();
	Scene* activeScene = sceneManager.getActiveScene();
	GameObject* tilemapObj = activeScene->getGameObject("LevelTilemap");
	GridComponent* gridComp =
		tilemapObj ? tilemapObj->getComponent<GridComponent>() : nullptr;
	Vector2 sampleBlockedCell{10.0, 7.0};
	bool sampleCellBlocked = true;

	while ( running && renderer.isOpen() ) {
		input->update();

		// Toggle debug rendering with 'G' key
		if ( input->wasKeyPressed(KeyCode::G) && gridComp != nullptr ) {
			bool currentState = gridComp->isDebugRenderEnabled();
			gridComp->setDebugRenderEnabled(!currentState);
			std::cout << "Grid debug rendering: "
					  << (currentState ? "OFF" : "ON") << std::endl;
		}

		// Toggle diagonal links with 'D' key
		if ( input->wasKeyPressed(KeyCode::D) && gridComp != nullptr ) {
			bool currentState = gridComp->isDebugShowDiagonalLinks();
			gridComp->setDebugShowDiagonalLinks(!currentState);
			std::cout << "Grid diagonal links: "
					  << (currentState ? "OFF" : "ON") << std::endl;
		}

		// Toggle sample blocked cell with 'B' key
		if ( input->wasKeyPressed(KeyCode::B) && gridComp != nullptr ) {
			if ( sampleCellBlocked ) {
				gridComp->unblockCell(sampleBlockedCell);
				std::cout << "Sample blocked cell: UNBLOCKED\n";
			} else {
				gridComp->blockCell(sampleBlockedCell);
				std::cout << "Sample blocked cell: BLOCKED\n";
			}
			sampleCellBlocked = !sampleCellBlocked;
		}

		if ( input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE) ) {
			running = false;
		}

		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime =
			static_cast<float>(currentTicks - lastTicks) / 1000.0f;
		lastTicks = currentTicks;

		sceneManager.update(deltaTime);
		sceneManager.buildRenderQueue(renderQueue);
		executeRenderQueue(renderer, renderQueue);

		SDL_Delay(16);
	}

	renderer.close();
	return 0;
}
