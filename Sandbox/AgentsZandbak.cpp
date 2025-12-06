

#include "AgentsZandbak.h"

#include "SimpleMoveBehaviour.h"
#include "AI/Agent.h"
#include "AI/Modules/AvoidTargetModule.h"
#include "AI/Modules/FollowTargetModule.h"
#include "AI/Modules/WanderModule.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"

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
    followRenderer->setRectangle({30, 30});
    followRenderer->setColor(Color::darkRed());
    Agent* followAgent = followEnemy->addComponent<Agent>();
    followAgent->setRotationTurnRate(360);
    followAgent->setMaxSpeed(300);
    followAgent->addAgentModule<FollowTargetModule>(100, *player->getTransform());
    followAgent->addAgentModule<WanderModule>(35, 25.0f, 100.0f, 10.0f);



    /// Avoid enemy:
    auto avoidEnemy = std::make_unique<GameObject>();
    avoidEnemy->getTransform()->setPosition({250, 250});
    auto avoidRenderer = avoidEnemy->addComponent<ShapeRenderer>();
    avoidRenderer->setRectangle({30, 30});
    avoidRenderer->setColor(Color::darkBlue());

    Agent* avoidAgent = avoidEnemy->addComponent<Agent>();
    avoidAgent->setMaxSpeed(100);
    avoidAgent->addAgentModule<AvoidTargetModule>(100, *player->getTransform(), 200.0f);
    avoidAgent->addAgentModule<WanderModule>(50, 25.0f, 100.0f, 10.0f);

    /// Construct and return scne
    auto scene = std::make_unique<Scene>("AgentsZandbak");

    scene->addGameObject(std::move(avoidEnemy));
    scene->addGameObject(std::move(player));
    scene->addGameObject(std::move(followEnemy));

    return std::move(scene);
}
