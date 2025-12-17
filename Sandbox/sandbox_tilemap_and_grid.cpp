#include "Assets/AssetManager.h"
#include "Assets/TilemapAsset.h"
#include "Behaviour/Behaviour.h"
#include "Component/GridComponent.h"
#include "Component/ShapeRenderer.h"
#include "Component/TilemapComponent.h"
#include "Component/Transform.h"
#include "../Engine/inc/Core/Options/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Math/Vector2.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

/**
 * @brief Behavior class that handles input for the tilemap/grid demo.
 *
 * Handles:
 * - G: Toggle grid debug rendering
 * - D: Toggle diagonal links display
 * - B: Toggle sample blocked cell
 * - ESC: Exit
 */
class TilemapInputBehaviour: public Behaviour
{
   public:
	explicit TilemapInputBehaviour(Scene* scene)
		: scene(scene),
		  inputManager(nullptr),
		  sampleCellBlocked(true),
		  sampleBlockedCell{10.0, 7.0}
	{
	}

	~TilemapInputBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();

		// Get grid component from scene
		if ( scene != nullptr )
		{
			GameObject* tilemapObj = scene->getGameObject("LevelTilemap");
			if ( tilemapObj != nullptr )
			{
				gridComponent = tilemapObj->getComponent<GridComponent>();
			}
		}
	}

	void update(float deltaTime, GameWorld* world) override
	{
		(void)deltaTime;
		(void)world;

		if ( inputManager == nullptr )
		{
			return;
		}

		// Toggle debug rendering with 'G' key
		if ( inputManager->wasKeyPressed(KeyCode::G) &&
			 gridComponent != nullptr )
		{
			bool currentState = gridComponent->isDebugRenderEnabled();
			gridComponent->setDebugRenderEnabled(!currentState);
			std::cout << "Grid debug rendering: "
					  << (currentState ? "OFF" : "ON") << std::endl;
		}

		// Toggle diagonal links with 'D' key
		if ( inputManager->wasKeyPressed(KeyCode::D) &&
			 gridComponent != nullptr )
		{
			bool currentState = gridComponent->isDebugShowDiagonalLinks();
			gridComponent->setDebugShowDiagonalLinks(!currentState);
			std::cout << "Grid diagonal links: "
					  << (currentState ? "OFF" : "ON") << std::endl;
		}

		// Toggle sample blocked cell with 'B' key
		if ( inputManager->wasKeyPressed(KeyCode::B) &&
			 gridComponent != nullptr )
		{
			if ( sampleCellBlocked )
			{
				gridComponent->unblockCell(sampleBlockedCell);
				std::cout << "Sample blocked cell: UNBLOCKED\n";
			}
			else
			{
				gridComponent->blockCell(sampleBlockedCell);
				std::cout << "Sample blocked cell: BLOCKED\n";
			}
			sampleCellBlocked = !sampleCellBlocked;
		}

		// Handle ESC key for exit
		if ( inputManager->wasKeyPressed(KeyCode::ESCAPE) )
		{
			inputManager->signalQuit();
		}
	}

   private:
	Scene* scene;
	InputManager* inputManager;
	GridComponent* gridComponent = nullptr;
	bool sampleCellBlocked;
	Vector2 sampleBlockedCell;
};

#undef main

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	ApplicationSpecifications spec = {};
	spec.networkingOptions.port = 8080;
	spec.networkingOptions.serverIP = "127.0.0.1";
	spec.networkingOptions.mode = EngineMode::CLIENT;
	spec.networkingOptions.tickRate = 60;
	spec.renderBackend = RenderBackend::SDL;
	spec.windowOptions = {"Tilemap Example", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	// Create a scene
	auto tilemapScene = std::make_unique<Scene>("TilemapScene");

	// Create AssetManager and load tilemap
	AssetManager assetManager;
	std::string tilemapPath = "Sandbox/Assets/level1_tilemap.csv";

	assetManager.add(tilemapPath, std::make_unique<TilemapAsset>());
	if ( !assetManager.load(tilemapPath) )
	{
		std::cerr << "Failed to load tilemap: " << tilemapPath << std::endl;
		return 1;
	}

	TilemapAsset* tilemap =
		dynamic_cast<TilemapAsset*>(assetManager.get(tilemapPath));
	if ( tilemap == nullptr )
	{
		std::cerr << "Failed to get tilemap asset" << std::endl;
		return 1;
	}

	// Create a GameObject for the tilemap
	auto tilemapObject = std::make_unique<GameObject>();
	tilemapObject->setName("LevelTilemap");

	// Add TilemapComponent
	auto* tilemapComponent = tilemapObject->addComponent<TilemapComponent>();
	if ( tilemapComponent == nullptr )
	{
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
	if ( gridComponent != nullptr )
	{
		gridComponent->setTilemapComponent(tilemapComponent);
		gridComponent->setWalkableTileIds({0, 2});

		// Set movement costs/weights for pathfinding
		// 1.0 = normal speed, higher = slower, lower = faster
		gridComponent->setTileWeight(0, 1.0);  // Low grass: normal
		gridComponent->setTileWeight(2, 2.0);  // High grass: 2x slower

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

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	inputHandler->addComponent<TilemapInputBehaviour>(tilemapScene.get());

	// Add to scene
	tilemapScene->addGameObject(std::move(tilemapObject));
	if ( blockedBoulder != nullptr )
	{
		// Append after the tilemap so it renders on top
		tilemapScene->addGameObject(std::move(blockedBoulder));
	}
	tilemapScene->addGameObject(std::move(inputHandler));

	// Example: Query grid information
	if ( tilemapComponent->isReady() )
	{
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

	// Test weight queries after retrieving grid component from scene
	if ( gridComponent != nullptr )
	{
		std::cout << "\nTile weights:\n";
		std::cout << "  Tile 0 (grass): " << gridComponent->getTileWeight(0)
				  << "\n";
		std::cout << "  Tile 2 (high grass): "
				  << gridComponent->getTileWeight(2) << "\n";
		std::cout << "  Tile 3 (water): " << gridComponent->getTileWeight(3)
				  << "\n";
		std::cout << "  Cell (3,3) weight: "
				  << gridComponent->getCellWeight({3.0, 3.0}) << "\n";
	}

	std::cout << "\nControls:\n";
	std::cout << "  G - Toggle grid debug rendering\n";
	std::cout << "  D - Toggle diagonal links\n";
	std::cout << "  B - Toggle sample blocked cell\n";
	std::cout << "  ESC - Exit\n\n";

	game->addScene(std::move(tilemapScene));
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}
