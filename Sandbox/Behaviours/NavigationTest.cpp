//
// Created by samle on 10/12/2025.
//
#include "NavigationTest.h"

#include "AI/Agent.h"
#include "AI/Modules/FollowPathModule.h"
#include "AI/Navigation/Pathfinding/PathRenderer.h"
#include "Component/GridComponent.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Networking/NetworkBuilder.h"
#include "Scene/Scene.h"

#include <iostream>
#include <ostream>

namespace
{
bool requestPathForAgent(Agent& agent, const Transform& target,
						 PathRenderer* renderer)
{
	if ( !agent.requestPath(target.getPosition()) ) return false;

	if ( renderer != nullptr ) renderer->setPath(agent.getCurrentPath());

	return true;
}

bool isGameObjectInScene(GameObject* object, Scene* scene)
{
	if ( object == nullptr || scene == nullptr ) return false;

	return scene->getGameObject(object->getGameObjectHandle()) == object;
}
}  // namespace

void NavigationTest::onAwake()
{
	if ( gameObject->hasComponent<PathRenderer>() )
	{
		pathRenderer = gameObject->getComponent<PathRenderer>();
	}
	else
	{
		std::cout << "[NavigationTest component] No PathRenderer component "
					 "exists on this GameObbject, consider adding it."
				  << std::endl;
	}
}

void NavigationTest::update(double deltaTime, const GameWorld& world)
{
	const bool clientWithoutServer = world.isClient() && !world.isServer();

	input = world.input;
	if ( input == nullptr ) return;

	// In a pure client build send the network command, but continue running the
	// local logic so the sandbox works without a server too.
	if ( clientWithoutServer && input->wasKeyPressed(KeyCode::SPACE) )
		callCommand("RequestPath");

	resolveReferences();

	if ( agent == nullptr || target == nullptr ) return;

	if ( input->wasKeyPressed(KeyCode::SPACE) ) handlePathRequest();

	if ( gridComponent == nullptr || !agent->hasPath() )
	{
		wasAtTarget = false;
		return;
	}

	const Transform* agentTransform = agent->getTransform();
	if ( agentTransform == nullptr )
	{
		wasAtTarget = false;
		return;
	}

	const float arrivalDistance = agent->getArrivingDistance();
	const float distanceToTarget =
		Vector2::distance(agentTransform->getPosition(), target->getPosition());
	const bool atTarget = distanceToTarget <= arrivalDistance;

	wasAtTarget = atTarget;
}

void NavigationTest::registerNetworkMethods(NetworkBuilder& builder)
{
	builder.command("RequestPath",
					[this](ReadArchive&) { handlePathRequest(); });
}

void NavigationTest::handlePathRequest()
{
	if ( !gameWorld || (!gameWorld->isServer() && gameWorld->isClient()) )
		return;

	resolveReferences();
	if ( agent == nullptr || target == nullptr ) return;

	requestPathForAgent(*agent, *target, pathRenderer);
}

void NavigationTest::serialize(WriteArchive& archive) const
{
	archive.process(const_cast<std::string&>(agentName));
	archive.process(const_cast<std::string&>(targetName));
	archive.process(const_cast<std::string&>(gridName));
	archive.process(const_cast<float&>(agentMaxSpeed));
	archive.process(const_cast<float&>(agentTurnRate));
	archive.process(const_cast<float&>(agentArriveDistance));
	archive.process(const_cast<float&>(followPathWeight));
	archive.process(const_cast<float&>(followPathRadius));
}

void NavigationTest::deserialize(ReadArchive& archive)
{
	archive.process(agentName);
	archive.process(targetName);
	archive.process(gridName);
	archive.process(agentMaxSpeed);
	archive.process(agentTurnRate);
	archive.process(agentArriveDistance);
	archive.process(followPathWeight);
	archive.process(followPathRadius);

	agent = nullptr;
	target = nullptr;
	gridComponent = nullptr;
	wasAtTarget = false;
	agentConfigured = false;
}

void NavigationTest::setTarget(Transform& targetTransform)
{
	target = &targetTransform;
	if ( GameObject* owner = targetTransform.getGameObject() )
	{
		targetName = owner->getName();
	}
	wasAtTarget = false;
}

void NavigationTest::setAgent(Agent& newAgent)
{
	agent = &newAgent;
	if ( GameObject* owner = newAgent.getGameObject() )
	{
		agentName = owner->getName();
	}
	agentConfigured = false;
}

void NavigationTest::setGridComponent(GridComponent& grid)
{
	gridComponent = &grid;
	if ( GameObject* owner = grid.getGameObject() )
	{
		gridName = owner->getName();
	}
}

void NavigationTest::setAgentConfig(float maxSpeed, float turnRate,
									float arrivingDistance,
									float followPathWeightValue,
									float followPathRadiusValue)
{
	agentMaxSpeed = maxSpeed;
	agentTurnRate = turnRate;
	agentArriveDistance = arrivingDistance;
	followPathWeight = followPathWeightValue;
	followPathRadius = followPathRadiusValue;
	agentConfigured = false;
}

void NavigationTest::resolveReferences()
{
	if ( gameObject == nullptr ) return;

	Scene* scene = gameObject->getScene();
	if ( scene == nullptr ) return;

	const bool agentValid =
		agent != nullptr && isGameObjectInScene(agent->getGameObject(), scene);
	if ( !agentValid )
	{
		Agent* resolvedAgent = nullptr;
		if ( !agentName.empty() )
		{
			if ( GameObject* agentObj = scene->getGameObject(agentName) )
			{
				resolvedAgent = agentObj->getComponent<Agent>();
			}
		}
		if ( resolvedAgent != agent )
		{
			agent = resolvedAgent;
			agentConfigured = false;
		}
	}

	const bool targetValid =
		target != nullptr &&
		isGameObjectInScene(target->getGameObject(), scene);
	if ( !targetValid )
	{
		Transform* resolvedTarget = nullptr;
		if ( !targetName.empty() )
		{
			if ( GameObject* targetObj = scene->getGameObject(targetName) )
			{
				resolvedTarget = targetObj->getTransform();
			}
		}
		target = resolvedTarget;
		wasAtTarget = false;
	}

	const bool gridValid =
		gridComponent != nullptr &&
		isGameObjectInScene(gridComponent->getGameObject(), scene);
	if ( !gridValid )
	{
		GridComponent* resolvedGrid = nullptr;
		if ( !gridName.empty() )
		{
			if ( GameObject* gridObj = scene->getGameObject(gridName) )
			{
				resolvedGrid = gridObj->getComponent<GridComponent>();
			}
		}
		gridComponent = resolvedGrid;
	}

	configureAgentIfNeeded();
}

void NavigationTest::configureAgentIfNeeded()
{
	if ( agentConfigured || agent == nullptr ) return;

	agent->setMaxSpeed(agentMaxSpeed);
	agent->setRotationTurnRate(agentTurnRate);
	agent->setArrivingDistance(agentArriveDistance);

	if ( !agent->hasAgentModule<FollowPathModule>() )
	{
		agent->addAgentModule<FollowPathModule>(followPathWeight,
												followPathRadius);
	}
	else
	{
		agent->setModuleWeight<FollowPathModule>(followPathWeight);
	}

	agentConfigured = true;
}
