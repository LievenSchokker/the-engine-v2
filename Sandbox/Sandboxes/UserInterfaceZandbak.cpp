#include "UserInterfaceZandbak.h"

#include "Component/Profiler/Profiler.h"
#include "Component/UIElement/UIPanelElement.h"
#include "Component/UIElement/UIProgressBar.h"
#include "Scene/Scene.h"


std::unique_ptr<Scene> UserInterfaceZandbak::getScene()
{
    auto scene = std::make_unique<Scene>("PathFindingZandbak");
	auto menu = std::make_unique<GameObject>();
	auto panel = menu->addComponent<UIPanelElement>(100, 0, 300, 300, "Menu");
	const auto bar = menu->addComponent<UIProgressBar>(0,0, 300, 300);
	bar->setBackgroundColor(Color::brown());
	scene->addGameObject(std::move(menu));



	return scene;
}


