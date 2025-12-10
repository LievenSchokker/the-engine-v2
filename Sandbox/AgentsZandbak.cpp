

#include "AgentsZandbak.h"

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
    avoidAgent->setRotationTurnRate(180);

    avoidAgent->setMaxSpeed(100);
    avoidAgent->addAgentModule<AvoidTargetModule>(100, *player->getTransform(), 200.0f);
    avoidAgent->addAgentModule<WanderModule>(50, 25.0f, 100.0f, 10.0f);

    /// Obstcle:
    auto obstacle = std::make_unique<GameObject>("Obstacle");
    auto obstacleRender = obstacle->addComponent<ShapeRenderer>();
    float width = 50.0f;
    float height = 50.0f;
    obstacleRender->setRectangle({width,height});
    obstacle->getTransform()->setPosition({100, 300});
    obstacleRender->setColor(Color::fromRGBA(255,255,255,50));
    auto navObstacle = obstacle->addComponent<NavigationObstacle>();
    navObstacle->useManualBounds({25, 25});
    navObstacle->setBoundsOffset({5, 0});

    /// Construct and return scne
    auto scene = std::make_unique<Scene>("AgentsZandbak");

    scene->addGameObject(std::move(avoidEnemy));
    scene->addGameObject(std::move(player));
    scene->addGameObject(std::move(followEnemy));
    scene->addGameObject(std::move(obstacle));
    // scene->addGameObject(std::move(gridGO));
    return std::move(scene);
}
