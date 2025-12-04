

#include "AgentsZandbak.h"

#include "SimpleMoveBehaviour.h"
#include "AI/Agent.h"
#include "AI/Modules/FollowTargetModule.h"
#include "Component/ShapeRenderer.h"
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

    /// Agent:
    auto enemy = std::make_unique<GameObject>();
    auto enemyRenderer = enemy->addComponent<ShapeRenderer>();
    enemyRenderer->setCircle(50);
    enemyRenderer->setColor(Color(255,0,0));
    Agent* agent = enemy->addComponent<Agent>();
    agent->setMaxModuleForceMagnitude(10);
    agent->setMaxVelocityMagnitude(100);
    agent->addAgentModule<FollowTargetModule>(10, *player->getTransform());

    /// Construct and return scne
    auto scene = std::make_unique<Scene>("AgentsZandbak");

    scene->addGameObject(std::move(player));
    scene->addGameObject(std::move(enemy));

    return std::move(scene);
}
