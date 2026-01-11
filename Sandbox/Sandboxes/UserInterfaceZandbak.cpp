#include "UserInterfaceZandbak.h"

#include "Component/Profiler/Profiler.h"
#include "Component/UIElement/UIButton.h"
#include "Component/UIElement/UIPanelElement.h"
#include "Component/UIElement/UIProgressBar.h"
#include "Component/UIElement/UISpacer.h"
#include "Component/UIElement/UITextElement.h"
#include "Scene/Scene.h"

std::unique_ptr<Scene> UserInterfaceZandbak::getScene()
{
    auto scene = std::make_unique<Scene>("UIStressTest");

    auto menu1 = std::make_unique<GameObject>();
    auto panel1 = menu1->addComponent<UIPanelElement>(20, 20, 280, 200, "THICC BOI");
    panel1->setPanelId(1);
    panel1->setBorder(true);
    panel1->setMinimizable(true);
    panel1->setBackgroundColor(Color(60, 20, 80, 250));
    panel1->setBorderColor(Color(255, 0, 255, 255));
    panel1->setBorderThickness(8.0f);
    panel1->setPadding(25.0f);
    panel1->setSpacing(15.0f);
    panel1->setTitleBackgroundColor(Color(120, 0, 120, 255));
    panel1->setTitleTextColor(Color(255, 255, 0, 255));
    panel1->setTitleHeight(40.0f);
    panel1->setTitlePadding(10.0f);
    panel1->setRowHeight(40.0f);
    panel1->setColumns(1);
	panel1->setDock(UIDock::BottomRight);

	auto label1 = menu1->addComponent<UITextElement>(0, 0, 200, 30, "MAXIMUM THICCNESS");
    label1->setPanelId(1);
    label1->setAlignment(Alignment::Center);
    label1->setColor(Color(255, 200, 255, 255));

    auto bar1 = menu1->addComponent<UIProgressBar>(0, 0, 200, 30);
    bar1->setPanelId(1);
    bar1->setValue(0.69f);
    bar1->setBarColor(Color(255, 0, 255, 255));
    bar1->setBackgroundColor(Color(30, 0, 30, 255));

    auto bar2 = menu1->addComponent<UIProgressBar>(0, 0, 200, 30);
    bar2->setPanelId(1);
    bar2->setValue(0.42f);
    bar2->setBarColor(Color(255, 255, 0, 255));
    bar2->setBackgroundColor(Color(50, 50, 0, 255));

    auto menu2 = std::make_unique<GameObject>();
    auto panel2 = menu2->addComponent<UIPanelElement>(320, 20, 200, 400, "RAZOR");
    panel2->setPanelId(2);
    panel2->setBorder(true);
    panel2->setMinimizable(true);
    panel2->setBackgroundColor(Color(5, 5, 5, 255));
    panel2->setBorderColor(Color(255, 0, 0, 255));
    panel2->setBorderThickness(1.0f);
    panel2->setPadding(2.0f);
    panel2->setSpacing(1.0f);
    panel2->setTitleBackgroundColor(Color(80, 0, 0, 255));
    panel2->setTitleTextColor(Color(255, 50, 50, 255));
    panel2->setTitleHeight(16.0f);
    panel2->setTitlePadding(2.0f);
    panel2->setRowHeight(18.0f);
    panel2->setColumns(2);

    auto razorBar1 = menu2->addComponent<UIProgressBar>(0, 0, 80, 10);
    razorBar1->setPanelId(2);
    razorBar1->setValue(0.9f);
    razorBar1->setBarColor(Color(255, 0, 0, 255));
    razorBar1->setBackgroundColor(Color(40, 0, 0, 255));

    auto razorBar2 = menu2->addComponent<UIProgressBar>(0, 0, 80, 10);
    razorBar2->setPanelId(2);
    razorBar2->setValue(0.7f);
    razorBar2->setBarColor(Color(255, 100, 0, 255));
    razorBar2->setBackgroundColor(Color(40, 20, 0, 255));

    auto razorBar3 = menu2->addComponent<UIProgressBar>(0, 0, 80, 10);
    razorBar3->setPanelId(2);
    razorBar3->setValue(0.5f);
    razorBar3->setBarColor(Color(255, 200, 0, 255));
    razorBar3->setBackgroundColor(Color(40, 40, 0, 255));

    auto razorBar4 = menu2->addComponent<UIProgressBar>(0, 0, 80, 10);
    razorBar4->setPanelId(2);
    razorBar4->setValue(0.3f);
    razorBar4->setBarColor(Color(200, 255, 0, 255));
    razorBar4->setBackgroundColor(Color(30, 40, 0, 255));

    auto menu3 = std::make_unique<GameObject>();
    auto panel3 = menu3->addComponent<UIPanelElement>(540, 20, 250, 350, "GHOST");
    panel3->setPanelId(3);
    panel3->setBorder(true);
    panel3->setMinimizable(true);
    panel3->setClosable(true);
    panel3->setBackgroundColor(Color(100, 150, 200, 80));
    panel3->setBorderColor(Color(200, 220, 255, 100));
    panel3->setBorderThickness(4.0f);
    panel3->setPadding(15.0f);
    panel3->setSpacing(10.0f);
    panel3->setTitleBackgroundColor(Color(50, 100, 150, 120));
    panel3->setTitleTextColor(Color(220, 240, 255, 200));
    panel3->setTitleHeight(30.0f);
    panel3->setRowHeight(28.0f);
    panel3->setColumns(1);

	auto ghostText = menu3->addComponent<UITextElement>(0, 0, 200, 30, "I am barely here...");
    ghostText->setPanelId(3);
    ghostText->setAlignment(Alignment::Center);
    ghostText->setColor(Color(200, 220, 255, 180));

    auto ghostBar = menu3->addComponent<UIProgressBar>(0, 0, 200, 20);
    ghostBar->setPanelId(3);
    ghostBar->setValue(0.33f);
    ghostBar->setBarColor(Color(150, 200, 255, 150));
    ghostBar->setBackgroundColor(Color(50, 80, 120, 100));

    auto menu4 = std::make_unique<GameObject>();
    auto panel4 = menu4->addComponent<UIPanelElement>(20, 540, 770, 200, "RAINBOW NIGHTMARE");
    panel4->setPanelId(4);
    panel4->setBorder(true);
    panel4->setMinimizable(true);
    panel4->setBackgroundColor(Color(20, 20, 30, 240));
    panel4->setBorderColor(Color(255, 255, 255, 255));
    panel4->setBorderThickness(3.0f);
    panel4->setPadding(10.0f);
    panel4->setSpacing(8.0f);
    panel4->setTitleBackgroundColor(Color(40, 40, 50, 255));
    panel4->setTitleTextColor(Color(255, 255, 255, 255));
    panel4->setTitleHeight(28.0f);
    panel4->setRowHeight(35.0f);
    panel4->setColumns(3);

    auto redBar1 = menu4->addComponent<UIProgressBar>(0, 0, 200, 25);
    redBar1->setPanelId(4);
    redBar1->setValue(1.0f);
    redBar1->setBarColor(Color(255, 0, 0, 255));
    redBar1->setBackgroundColor(Color(50, 0, 0, 255));

    auto greenBar1 = menu4->addComponent<UIProgressBar>(0, 0, 200, 25);
    greenBar1->setPanelId(4);
    greenBar1->setValue(0.8f);
    greenBar1->setBarColor(Color(0, 255, 0, 255));
    greenBar1->setBackgroundColor(Color(0, 50, 0, 255));

    auto blueBar1 = menu4->addComponent<UIProgressBar>(0, 0, 200, 25);
    blueBar1->setPanelId(4);
    blueBar1->setValue(0.6f);
    blueBar1->setBarColor(Color(0, 0, 255, 255));
    blueBar1->setBackgroundColor(Color(0, 0, 50, 255));

    auto redBar2 = menu4->addComponent<UIProgressBar>(0, 0, 200, 25);
    redBar2->setPanelId(4);
    redBar2->setValue(0.7f);
    redBar2->setBarColor(Color(255, 100, 100, 255));
    redBar2->setBackgroundColor(Color(60, 20, 20, 255));

    auto greenBar2 = menu4->addComponent<UIProgressBar>(0, 0, 200, 25);
    greenBar2->setPanelId(4);
    greenBar2->setValue(0.5f);
    greenBar2->setBarColor(Color(100, 255, 100, 255));
    greenBar2->setBackgroundColor(Color(20, 60, 20, 255));

    auto blueBar2 = menu4->addComponent<UIProgressBar>(0, 0, 200, 25);
    blueBar2->setPanelId(4);
    blueBar2->setValue(0.3f);
    blueBar2->setBarColor(Color(100, 100, 255, 255));
    blueBar2->setBackgroundColor(Color(20, 20, 60, 255));

	menu4->addComponent<UIButton>();
    // Add in order (back to front)
    scene->addGameObject(std::move(menu4));
    scene->addGameObject(std::move(menu3));
    scene->addGameObject(std::move(menu2));
    scene->addGameObject(std::move(menu1));

    return scene;
}