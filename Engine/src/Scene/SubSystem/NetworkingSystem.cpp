#include "Scene/SubSystems/NetworkSystem.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Component/Transform.h"
#include "Component/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkSpawnManager.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/IPhysicsWorld.h"
#include "Assets/SpritesheetLoader.h"
#include "Component/SpriteComponent.h"
#include "Scene/SubSystems/BehaviourSystem.h"

#include <iostream>

#include "Networking/Server/Server.h"

void NetworkSystem::configure(NetworkSpawnManager *spawnMgr, BehaviourSystem *behSys)
{
	spawnManager = spawnMgr;
	behaviourSystem = behSys;
	configured = (spawnManager != nullptr);
}

bool NetworkSystem::isConfigured() const { return configured; }

void NetworkSystem::processSceneForNetwork(Scene &scene, GameWorld &gameWorld)
{
	if (!configured) return;

	if (gameWorld.server != nullptr) { processForServer(scene, gameWorld); } else { processForClient(scene); }
}

void NetworkSystem::processForServer(Scene &scene, GameWorld &gameWorld)
{
	if (spawnManager)
	{
		spawnManager->clearPrefabLibrary();
	}
	std::vector<GameObject *> networkObjects;

	scene.forEachGameObject([&](GameObject &obj)
	{
		if (hasNetworkBehaviour(obj) && !hasNetworkIdentity(obj)) { networkObjects.push_back(&obj); }
	});

	std::cout << "[NetworkSystem] Found " << networkObjects.size()
			<< " network objects to process" << std::endl;

	if (networkObjects.empty()) return;

	for (GameObject *obj: networkObjects)
	{
		auto behaviours = obj->getComponents<NetworkBehaviour>();

		SpawnOnType finalAuthority = SpawnOnType::ServerSpawn;
		bool hasClientAuthority = false;
		bool hasServerAuthority = false;

		for (auto *behaviour: behaviours)
		{
			if (behaviour->getAuthorityType() == SpawnOnType::ClientSpawn) { hasClientAuthority = true; } else
			{
				hasServerAuthority = true;
			}
		}

		if (hasClientAuthority && hasServerAuthority)
		{
			std::cerr << "[NetworkSystem] ERROR: GameObject '" << obj->getName()
					<< "' has NetworkBehaviours with mixed authority types!\n";

			// Log which behaviours have which authority
			for (auto *behaviour: behaviours)
			{
				std::cerr << "  - " << typeid(*behaviour).name()
						<< ": " << (behaviour->getAuthorityType() == SpawnOnType::ClientSpawn
										? "ClientAuthority"
										: "ServerAuthority")
						<< "\n";
			}

			std::cerr << "  Defaulting all to ServerAuthority." << std::endl;

			for (auto *behaviour: behaviours) { behaviour->setAuthorityType(SpawnOnType::ServerSpawn); }
			finalAuthority = SpawnOnType::ServerSpawn;
		} else if (hasClientAuthority) { finalAuthority = SpawnOnType::ClientSpawn; }

		if (finalAuthority == SpawnOnType::ClientSpawn)
		{
			// ClientAuthority: just register as prefab
			auto extracted = scene.extractGameObject(obj->getGameObjectHandle());
			if (!extracted) continue;

			spawnManager->addToPrefabLibrary(std::move(extracted));
			continue;
		}

		// ServerAuthority: register as prefab AND spawn
		Vector2 spawnPosition = obj->getTransform()->getPosition();
		auto extracted = scene.extractGameObject(obj->getGameObjectHandle());
		if (!extracted) continue;

		extracted->getTransform()->setPosition(spawnPosition);

		if (!extracted->getComponent<NetworkIdentity>()) { extracted->addComponent<NetworkIdentity>(); }

		uint32_t assetId = spawnManager->addToPrefabLibrary(std::move(extracted));
		spawnManager->spawnObject(assetId, -1);
	}

	spawnClientObjectsForConnectedClients(gameWorld);
}

void NetworkSystem::spawnClientObjectsForConnectedClients(GameWorld& gameWorld)
{
	if (!spawnManager || !gameWorld.server)
	{
		return;
	}

	const auto& connectedClients = gameWorld.server->getConnectedClientIds();

	if (connectedClients.empty())
	{
		return;
	}

	for (const int clientId : connectedClients)
	{
		spawnManager->CheckNewClientSpawnObject(clientId);
	}
}

void NetworkSystem::processForClient(Scene &scene)
{
	std::vector<GameObject *> toProcess;

	scene.forEachGameObject([&](GameObject &obj)
	{
		if (hasNetworkBehaviour(obj) && !hasNetworkIdentity(obj)) { toProcess.push_back(&obj); }
	});

	if (toProcess.empty()) return;

	for (const GameObject *obj: toProcess)
	{
		auto extracted = scene.extractGameObject(obj->getGameObjectHandle());
		if (!extracted) continue;

		extracted->getTransform()->setPosition({0, 0});

		if (!extracted->getComponent<NetworkIdentity>()) { extracted->addComponent<NetworkIdentity>(); }

		spawnManager->addToPrefabLibrary(std::move(extracted));
	}
}

void NetworkSystem::applyNetworkSnapshot(
	const std::string &targetSceneName,
	std::vector<std::unique_ptr<GameObject> > &receivedObjects,
	Scene *&activeScene,
	GameWorld &gameWorld)
{
	if (!spawnManager || !activeScene) return;

	if (activeScene->getName() != targetSceneName)
	{
		std::cerr << "[NetworkSystem] ERROR: Scene mismatch! Target: " << targetSceneName
				<< " but activeScene is: " << activeScene->getName() << std::endl;
		return;
	}

	std::unordered_set<uint32_t> receivedNetIds;
	std::vector<std::pair<GameObject *, uint32_t> > toFixup;

	for (auto &received: receivedObjects)
	{
		if (!received) continue;

		auto *identity = received->getComponent<NetworkIdentity>();
		if (!identity) continue;

		uint32_t netId = identity->getNetId();
		receivedNetIds.insert(netId);

		GameObject *existing = spawnManager->getObjectByNetId(netId);

		if (existing) { updateExistingObject(existing, *received, gameWorld, toFixup, *activeScene); } else
		{
			spawnNewObject(std::move(received), gameWorld, toFixup, *activeScene);
		}
	}

	for (auto &[child, parentNetId]: toFixup)
	{
		if (GameObject *parent = spawnManager->getObjectByNetId(parentNetId)) { child->setParent(parent); }
	}

	removeStaleObjects(receivedNetIds, gameWorld, *activeScene);

	if (gameWorld.physics) { gameWorld.physics->applyNetworkSnapshot(); }
}

void NetworkSystem::updateExistingObject(
	GameObject *existing,
	GameObject &received,
	GameWorld &gameWorld,
	std::vector<std::pair<GameObject *, uint32_t> > &toFixup,
	Scene &activeScene)
{
	existing->copyStateFrom(received);

	// Sync physics
	if (gameWorld.physics)
	{
		auto *rigidBody = existing->getComponent<RigidBody>();
		auto *transform = existing->getTransform();

		if (rigidBody && transform)
		{
			gameWorld.physics->setBodyTransform(
				rigidBody,
				transform->getPosition(),
				transform->getRotationAngle()
			);
		}
	}

	// Handle parent fixup
	if (auto parentNetId = existing->consumePendingParentNetId()) { toFixup.push_back({existing, *parentNetId}); }

	// Handle inline children
	for (auto &child: existing->consumeInlineChildren())
	{
		GameObject *childPtr = child.get();
		activeScene.addGameObject(std::move(child));
		addInlineChildrenRecursive(childPtr, activeScene);
	}

	// Reload assets if needed
	if (gameWorld.getAssetManager()) { reloadAssetsForGameObject(existing, *gameWorld.getAssetManager()); }
}

void NetworkSystem::spawnNewObject(
	std::unique_ptr<GameObject> received,
	GameWorld &gameWorld,
	std::vector<std::pair<GameObject *, uint32_t> > &toFixup,
	Scene &activeScene)
{
	auto *identity = received->getComponent<NetworkIdentity>();
	uint32_t netId = identity ? identity->getNetId() : 0;

	if (identity)
	{
		identity->setWorld(&gameWorld);
		spawnManager->getNetworkIdentityRegistry().registerIdentity(identity);
	}

	std::vector<Behaviour *> behaviours = received->getAllBehaviours();
	GameObject *rawPtr = received.get();

	if (auto parentNetId = received->consumePendingParentNetId()) { toFixup.push_back({rawPtr, *parentNetId}); }

	activeScene.addGameObject(std::move(received));
	spawnManager->trackSpawnedObject(netId, rawPtr);

	if (behaviourSystem) { behaviourSystem->initialiseRuntimeBehaviours(behaviours, gameWorld); }

	if (gameWorld.getAssetManager()) { reloadAssetsForGameObject(rawPtr, *gameWorld.getAssetManager()); }

	if (identity) { identity->onNetworkSpawn(); }

	addInlineChildrenRecursive(rawPtr, activeScene);
}

void NetworkSystem::removeStaleObjects(
	const std::unordered_set<uint32_t> &receivedNetIds,
	GameWorld &gameWorld,
	Scene &activeScene)
{
	std::vector<uint32_t> toRemove;

	for (auto *identity: spawnManager->getNetworkIdentityRegistry().getAllIdentities())
	{
		if (!identity) continue;

		uint32_t netId = identity->getNetId();

		if (isLocallyOwned(identity, gameWorld)) continue;

		if (!receivedNetIds.contains(netId)) { toRemove.push_back(netId); }
	}

	for (uint32_t netId: toRemove)
	{
		// Get object directly from scene, not from spawnManager
		NetworkIdentity *identity = spawnManager->getNetworkIdentityRegistry().findByNetId(netId);
		if (!identity) continue;

		GameObject *object = identity->getGameObject();
		if (!object) continue;


		identity->onNetworkDespawn();
		spawnManager->getNetworkIdentityRegistry().unregisterIdentity(identity);
		spawnManager->untrackSpawnedObject(netId);

		activeScene.removeGameObject(object->getGameObjectHandle());
	}
}

void NetworkSystem::cleanupNetworkObjectsInScene(Scene &scene)
{
	if (!spawnManager) return;

	auto &registry = spawnManager->getNetworkIdentityRegistry();

	std::vector<std::pair<uint32_t, ObjectHandle> > toCleanup;

	scene.forEachGameObject([&](GameObject &obj)
	{
		auto *identity = obj.getComponent<NetworkIdentity>();
		if (identity && identity->getNetId() != 0)
		{
			toCleanup.push_back({identity->getNetId(), obj.getGameObjectHandle()});
		}
	});


	for (auto &[netId, handle]: toCleanup)
	{
		GameObject *obj = scene.getGameObject(handle);
		if (!obj) { continue; }
		auto *identity = obj->getComponent<NetworkIdentity>();
		if (identity)
		{
			identity->onNetworkDespawn();
			registry.unregisterIdentity(identity);
		}
		spawnManager->untrackSpawnedObject(netId);

		scene.removeGameObject(handle);
	}

	int remaining = 0;
	scene.forEachGameObject([&](GameObject &obj) { if (obj.getComponent<NetworkIdentity>()) { remaining++; } });
}

bool NetworkSystem::isLocallyOwned(NetworkIdentity *identity, const GameWorld &gameWorld) const
{
	if (!identity) return false;

	GameObject *go = identity->getGameObject();
	if (!go) return false;

	for (NetworkBehaviour *netBehaviour: go->getAllComponentsOfType<NetworkBehaviour>())
	{
		if (netBehaviour->getAuthorityType() == SpawnOnType::ClientSpawn &&
			identity->getOwnerId() == gameWorld.localClientId) { return true; }
	}

	return false;
}

void NetworkSystem::reloadAssetsForGameObject(GameObject *obj, AssetManager &assetManager)
{
	for (auto &component: obj->getComponents())
	{
		if (auto *sprite = dynamic_cast<SpriteComponent *>(component.get()))
		{
			if (sprite->getSprite() && sprite->getSprite()->isLoaded()) continue;

			std::string path = sprite->getPath();
			if (path.empty()) continue;

			SpritesheetLoader::loadSpritesheet(
				&assetManager, sprite, path, sprite->getSpritesheetDefinition()
			);
		}
	}
}

void NetworkSystem::addInlineChildrenRecursive(GameObject *obj, Scene &activeScene)
{
	for (auto &child: obj->consumeInlineChildren())
	{
		GameObject *childPtr = child.get();
		activeScene.addGameObject(std::move(child));
		addInlineChildrenRecursive(childPtr, activeScene);
	}
}

bool NetworkSystem::hasNetworkBehaviour(const GameObject &obj) const
{
	for (const auto &component: obj.getComponents())
	{
		if (dynamic_cast<NetworkBehaviour *>(component.get())) return true;
	}
	return false;
}

bool NetworkSystem::hasNetworkIdentity(const GameObject &obj) const
{
	return obj.getComponent<NetworkIdentity>() != nullptr;
}
