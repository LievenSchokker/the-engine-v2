

#include "AgentsZandbak.h"

#include <iostream>

#include "Game.h"
#include "SimpleMoveBehaviour.h"
#include "AI/Agent.h"
#include "AI/Modules/AvoidTargetModule.h"
#include "AI/Modules/FollowTargetModule.h"
#include "AI/Modules/WanderModule.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "AI/Navigation/BoundingBox.h"
#include "Behaviours/NavigationTest.h"

std::unique_ptr<Scene> AgentsZandbak::getScene()
{

    /// Player:
    auto player = std::make_unique<GameObject>();
    auto renderer = player->addComponent<ShapeRenderer>();
    renderer->setCircle(50);
    renderer->setColor(Color(0,255,0));
    auto movement = player->addComponent<SimpleMoveBehaviour>();
    movement->setMaxSpeed(250.0f);
    player->getTransform()->setPosition((Vector2{500, 500}));

    /// Follow enemy:
    auto followEnemy = std::make_unique<GameObject>();
    auto followRenderer = followEnemy->addComponent<ShapeRenderer>();
    followEnemy->getTransform()->setPosition({250, 150});
    followRenderer->setRectangle({30, 30});
    followRenderer->setColor(Color::darkRed());

    Agent* followAgent = followEnemy->addComponent<Agent>();
    followAgent->setRotationTurnRate(230);
    followAgent->setMaxSpeed(300);
    followAgent->addAgentModule<FollowTargetModule>(100, *player->getTransform(), 300.0f);
    followAgent->addAgentModule<WanderModule>(50, 25.0f, 150.0f, 5.0f);


    /// Avoid enemy:
    auto avoidEnemy = std::make_unique<GameObject>();
    avoidEnemy->getTransform()->setPosition({250, 250});
    auto avoidRenderer = avoidEnemy->addComponent<ShapeRenderer>();
    avoidRenderer->setRectangle({30, 30});
    avoidRenderer->setColor(Color::darkBlue());

    Agent* avoidAgent = avoidEnemy->addComponent<Agent>();
    Agent* avoidAgent2 = avoidEnemy->addComponent<Agent>();
    avoidAgent->setRotationTurnRate(180);

    avoidAgent->setMaxSpeed(100);
    avoidAgent->addAgentModule<AvoidTargetModule>(100, *player->getTransform(), 200.0f);
    avoidAgent->addAgentModule<WanderModule>(50, 25.0f, 100.0f, 10.0f);

    /// Ball:
    auto ball = std::make_unique<GameObject>();
    auto navTest = ball->addComponent<NavigationTest>();
    auto ballRender = ball->addComponent<ShapeRenderer>();
    ballRender->setCircle(25);
    ballRender->setColor(Color(0,0,255));
    ball->getTransform()->setPosition((Vector2{250, 10}));
    navTest->setTarget(*player->getTransform());
    std::cout << " Has: " << ball->getComponent<NavigationTest>() << std::endl;


    /// Construct and return scne
    auto scene = std::make_unique<Scene>("AgentsZandbak");

    for (auto& obstacle: createObstacles())
    {
        scene->addGameObject(std::move(obstacle));
    }
    scene->addGameObject(std::move(ball));
    scene->addGameObject(std::move(avoidEnemy));
    scene->addGameObject(std::move(player));
    scene->addGameObject(std::move(followEnemy));
    // scene->addGameObject(std::move(gridGO));
    return std::move(scene);
}

std::vector<std::unique_ptr<GameObject>> AgentsZandbak::createObstacles()
{
 std::vector<std::unique_ptr<GameObject>> obstacles;

    // Obstacle 1 - small
    auto obstacle1 = std::make_unique<GameObject>("Obstacle1");
    auto render1 = obstacle1->addComponent<ShapeRenderer>();
    float width1 = 100;
    float height1 = 100;
    render1->setRectangle({width1, height1});
    obstacle1->getTransform()->setPosition({150, 200});
    render1->setColor(Color::fromRGBA(255, 255, 255, 50));
    auto navObstacle1 = obstacle1->addComponent<NavigationObstacle>();
    navObstacle1->useManualBounds({width1 , height1 });
    navObstacle1->setBoundsOffset({0, 0});
    obstacles.push_back(std::move(obstacle1));

    // Obstacle 2 - medium
    auto obstacle2 = std::make_unique<GameObject>("Obstacle2");
    auto render2 = obstacle2->addComponent<ShapeRenderer>();
    float width2 = 160.0f;
    float height2 = 120.0f;
    render2->setRectangle({width2, height2});
    obstacle2->getTransform()->setPosition({400, 300});
    render2->setColor(Color::fromRGBA(255, 255, 255, 50));
    auto navObstacle2 = obstacle2->addComponent<NavigationObstacle>();
    navObstacle2->useManualBounds({width2 , height2 });
    navObstacle2->setBoundsOffset({0, 0});
    obstacles.push_back(std::move(obstacle2));

    // Obstacle 3 - tall
    auto obstacle3 = std::make_unique<GameObject>("Obstacle3");
    auto render3 = obstacle3->addComponent<ShapeRenderer>();
    float width3 = 80;
    float height3 = 240.0f;
    render3->setRectangle({width3, height3});
    obstacle3->getTransform()->setPosition({300, 150});
    render3->setColor(Color::fromRGBA(255, 255, 255, 50));
    auto navObstacle3 = obstacle3->addComponent<NavigationObstacle>();
    navObstacle3->useManualBounds({width3 , height3 });
    navObstacle3->setBoundsOffset({0, 0});
    obstacles.push_back(std::move(obstacle3));

    // Obstacle 4 - horizontal
    auto obstacle4 = std::make_unique<GameObject>("Obstacle4");
    auto render4 = obstacle4->addComponent<ShapeRenderer>();
    float width4 = 300.0f;
    float height4 = 60.0f;
    render4->setRectangle({width4, height4});
    obstacle4->getTransform()->setPosition({200, 400});
    render4->setColor(Color::fromRGBA(255, 255, 255, 50));
    auto navObstacle4 = obstacle4->addComponent<NavigationObstacle>();
    navObstacle4->useManualBounds({width4, height4 });
    navObstacle4->setBoundsOffset({0, 0});
    obstacles.push_back(std::move(obstacle4));

    return obstacles;
}
