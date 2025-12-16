#include "../Sandboxes/PathFindingZandbak.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Agent.h"
#include "AI/Navigation/Pathfinding/PathRenderer.h"
#include "../Behaviours/SimpleMoveBehaviour.h"
#include "../Behaviours/NavigationTest.h"
#include "AI/Modules/FollowPathModule.h"
#include "AI/Navigation/NavigationGridRenderer.h"

std::unique_ptr<Scene> PathFindingZandbak::getScene()
{
    auto scene = std::make_unique<Scene>("PathFindingZandbak");

    /// Player
    auto player = std::make_unique<GameObject>("Player");
    auto playerRenderer = player->addComponent<ShapeRenderer>();
    playerRenderer->setCircle(50);
    playerRenderer->setColor({0, 255, 0, 255});
    auto playerMovement = player->addComponent<SimpleMoveBehaviour>();
    playerMovement->setMaxSpeed(250.0f);
    player->getTransform()->setPosition({500, 500});

    /// Agent
    auto agentGO = std::make_unique<GameObject>("Agent");
    agentGO->getTransform()->setPosition({10, 10});

    auto agentRenderer = agentGO->addComponent<ShapeRenderer>();
    agentRenderer->setRectangle({30, 30});
    agentRenderer->setColor({0, 0, 255, 255});

    Agent* agentComp = agentGO->addComponent<Agent>();
    agentComp->setMaxSpeed(200.0f);
    agentComp->setRotationTurnRate(180);
    agentComp->addAgentModule<FollowPathModule>(200, 3.0f);


    /// Grid Renderer:
    auto gridGO = std::make_unique<GameObject>("GridRenderer");
    auto gridRenderer = gridGO->addComponent<NavigationGridRenderer>();

    // Add obstacles
    for (auto& obstacle : createObstacles())
        scene->addGameObject(std::move(obstacle));

    /// Test Object / PathRender:
    auto inputGO = std::make_unique<GameObject>("Input");
    auto navTest = inputGO->addComponent<NavigationTest>();
    auto pathRender = inputGO->addComponent<PathRenderer>();
    navTest->setAgent(*agentComp);
    navTest->setTarget(*player->getTransform());


    /// Add to scene:
    scene->addGameObject(std::move(inputGO));
    scene->addGameObject(std::move(player));
    scene->addGameObject(std::move(agentGO));
    scene->addGameObject(std::move(gridGO));

    return scene;
}


std::vector<std::unique_ptr<GameObject> > PathFindingZandbak::createObstacles()
{
    std::vector<std::unique_ptr<GameObject>> obstacles;

    auto obs1 = std::make_unique<GameObject>("Obstacle1");
    auto render1 = obs1->addComponent<ShapeRenderer>();
    render1->setRectangle({100, 100});
    render1->setColor({255, 255, 255, 50});
    obs1->getTransform()->setPosition({300, 300});
    auto navObs1 = obs1->addComponent<NavigationObstacle>();
    navObs1->useManualBounds({100, 100});
    obstacles.push_back(std::move(obs1));

    auto obs2 = std::make_unique<GameObject>("Obstacle2");
    auto render2 = obs2->addComponent<ShapeRenderer>();
    render2->setRectangle({150, 80});
    render2->setColor({255, 255, 255, 50});
    obs2->getTransform()->setPosition({600, 400});
    auto navObs2 = obs2->addComponent<NavigationObstacle>();
    navObs2->useManualBounds({150, 80});
    obstacles.push_back(std::move(obs2));

    return obstacles;
}
