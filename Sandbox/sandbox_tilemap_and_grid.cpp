#include "AI/Agent.h"
#include "AI/Modules/FollowPathModule.h"
#include "AI/Navigation/Pathfinding/PathRenderer.h"
#include "Assets/AssetManager.h"
#include "Assets/TilemapAsset.h"
#include "Behaviour/Behaviour.h"
#include "Behaviours/NavigationTest.h"
#include "Component/GridComponent.h"
#include "Component/ShapeRenderer.h"
#include "Component/TilemapComponent.h"
#include "Component/Transform.h"
#include "Core/GameWorld.h"
#include "Core/Options/ApplicationSpecifications.h"
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
#include <random>
#include <vector>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

namespace
{
std::mt19937& tilemapSandboxRng()
{
	static std::mt19937 engine{std::random_device{}()};
	return engine;
}

bool tryGetRandomWalkableCell(const GridComponent& grid, Vector2& outCell,
							  const Vector2* avoidCell = nullptr)
{
	const int width = grid.getGridWidth();
	const int height = grid.getGridHeight();
	if ( width <= 0 || height <= 0 )
	{
		return false;
	}

	std::vector<Vector2> candidates;
	candidates.reserve(static_cast<size_t>(width * height));

	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			Vector2 cell{static_cast<float>(x), static_cast<float>(y)};
			if ( !grid.isWalkable(cell) )
			{
				continue;
			}
			if ( avoidCell != nullptr && cell == *avoidCell )
			{
				continue;
			}
			candidates.push_back(cell);
		}
	}

	if ( candidates.empty() )
	{
		return false;
	}

	std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
	outCell = candidates[dist(tilemapSandboxRng())];
	return true;
}
}  // namespace

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
		: scene(scene), sampleCellBlocked(true), sampleBlockedCell{10.0, 7.0}
	{
	}

	~TilemapInputBehaviour() override = default;

	void onAwake() override
	{
		// Get grid component from scene
		if ( scene != nullptr )
		{
			GameObject* tilemapObj = scene->getGameObject("LevelTilemap");
			if ( tilemapObj != nullptr )
			{
				gridComponent = tilemapObj->getComponent<GridComponent>();
			}

			GameObject* navInputObj = scene->getGameObject("NavigationInput");
			if ( navInputObj != nullptr )
			{
				pathRenderer = navInputObj->getComponent<PathRenderer>();
			}
		}

		if ( gridComponent != nullptr && pathRenderer != nullptr )
		{
			pathRenderer->setRenderEnabled(
				gridComponent->isDebugRenderEnabled());
		}
	}

	void update(double deltaTime, const GameWorld& world) override
	{
		(void)deltaTime;
		(void)world;

		// Toggle debug rendering with 'G' key
		if ( world.input->wasKeyPressed(KeyCode::G) &&
			 gridComponent != nullptr )
		{
			bool currentState = gridComponent->isDebugRenderEnabled();
			bool newState = !currentState;
			gridComponent->setDebugRenderEnabled(newState);
			if ( pathRenderer != nullptr )
			{
				pathRenderer->setRenderEnabled(newState);
			}
			std::cout << "Grid debug rendering: " << (newState ? "ON" : "OFF")
					  << std::endl;
		}

		// Toggle sample blocked cell with 'B' key
		if ( world.input->wasKeyPressed(KeyCode::B) &&
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
		if ( world.input->wasKeyPressed(KeyCode::ESCAPE) )
		{
			world.input->signalQuit();
		}
	}

   private:
	Scene* scene;
	GridComponent* gridComponent = nullptr;
	PathRenderer* pathRenderer = nullptr;
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
	spec.networkingOptions.tickRate = 60;
	spec.renderSettings.renderBackend = RenderBackend::SDL;
	spec.renderSettings.windowOptions = {"Tilemap Example", false, SCREEN_WIDTH,
										 SCREEN_HEIGHT};
	spec.engineSystem = EngineSystem::Client;
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	// Create a scene
	auto tilemapScene = std::make_unique<Scene>("TilemapScene");

	// Create AssetManager and load tilemap
	AssetManager assetManager;
	std::string tilemapPath = "Assets/level1_tilemap.csv";

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

	// Add GridComponent for AI pathfinding (future use)
	auto* gridComponent = tilemapObject->addComponent<GridComponent>();
	std::unique_ptr<GameObject> blockedBoulder;
	std::unique_ptr<GameObject> agentObject;
	std::unique_ptr<GameObject> targetObject;
	std::unique_ptr<GameObject> navInputObject;
	Agent* agentComponent = nullptr;
	if ( gridComponent != nullptr )
	{
		gridComponent->setTilemapComponent(tilemapComponent);
		gridComponent->setWalkableTileIds({0});

		// Set movement costs/weights for pathfinding
		// 1.0 = normal speed, higher = slower, lower = faster
		gridComponent->setTileWeight(0, 1.0);  // Low grass: normal

		// Mark a sample blocked cell dynamically and visualize it with a
		// boulder
		Vector2 blockedCell{10.0, 7.0};
		gridComponent->blockCell(blockedCell);

		blockedBoulder = std::make_unique<GameObject>();
		blockedBoulder->setName("BlockedBoulder");
		auto* boulderRenderer = blockedBoulder->addComponent<ShapeRenderer>();
		boulderRenderer->setRectangle(tilemapComponent->getTileSize())
			.setColor(Color::gray())
			.setLayer(1);
		blockedBoulder->getTransform()->setPosition(
			tilemapComponent->cellToWorld(blockedCell));

		Vector2 agentCell = Vector2::zero();
		if ( !tryGetRandomWalkableCell(*gridComponent, agentCell) )
		{
			std::cout << "[TilemapSandbox] No walkable agent cell found; using "
						 "(0,0).\n";
		}

		Vector2 targetCell = agentCell;
		if ( !tryGetRandomWalkableCell(*gridComponent, targetCell, &agentCell) )
		{
			if ( !tryGetRandomWalkableCell(*gridComponent, targetCell) )
			{
				std::cout << "[TilemapSandbox] No walkable target cell found; "
							 "using agent cell.\n";
				targetCell = agentCell;
			}
		}

		if ( targetCell == agentCell )
		{
			std::cout << "[TilemapSandbox] Agent/target cells overlap; path "
						 "may be empty.\n";
		}

		// Create an agent that uses the tilemap-backed navigation surface
		agentObject = std::make_unique<GameObject>();
		agentObject->setName("TilemapAgent");
		auto* agentRenderer = agentObject->addComponent<ShapeRenderer>();
		agentRenderer->setRectangle({20.0f, 20.0f})
			.setColor(Color::red())
			.setLayer(1);

		agentComponent = agentObject->addComponent<Agent>();
		agentComponent->setMaxSpeed(150.0f);
		agentComponent->setRotationTurnRate(180.0f);
		agentComponent->setArrivingDistance(4.0f);
		agentComponent->addAgentModule<FollowPathModule>(150.0f, 4.0f);

		agentObject->getTransform()->setPosition(
			tilemapComponent->cellToWorld(agentCell));

		// Target that the agent will path towards
		targetObject = std::make_unique<GameObject>();
		targetObject->setName("Target");
		auto* targetRenderer = targetObject->addComponent<ShapeRenderer>();
		targetRenderer->setCircle(6.0f).setColor(Color::yellow()).setLayer(1);
		targetObject->getTransform()->setPosition(
			tilemapComponent->cellToWorld(targetCell));

		// Input + path visualization
		navInputObject = std::make_unique<GameObject>();
		navInputObject->setName("NavigationInput");
		auto* navTest = navInputObject->addComponent<NavigationTest>();
		auto* pathRenderer = navInputObject->addComponent<PathRenderer>();
		pathRenderer->setLayer(1).setRenderEnabled(false);
		navTest->setAgent(*agentComponent);
		navTest->setTarget(*targetObject->getTransform());
		navTest->setGridComponent(*gridComponent);
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
	if ( targetObject != nullptr )
	{
		tilemapScene->addGameObject(std::move(targetObject));
	}
	if ( agentObject != nullptr )
	{
		tilemapScene->addGameObject(std::move(agentObject));
	}
	if ( navInputObject != nullptr )
	{
		tilemapScene->addGameObject(std::move(navInputObject));
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
		std::cout << "  Cell (3,3) weight: "
				  << gridComponent->getCellWeight({3.0, 3.0}) << "\n";
	}

	std::cout << "\nControls:\n";
	std::cout << "  G - Toggle grid debug + path rendering\n";
	std::cout << "  B - Toggle sample blocked cell\n";
	std::cout << "  SPACE - Request path to target\n";
	std::cout << "  ESC - Exit\n\n";

	game->addScene(std::move(tilemapScene));
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}
