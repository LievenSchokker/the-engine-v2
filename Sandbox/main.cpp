//
// #include "Core/ApplicationSpecifications.h"
// #include "EntryPoint.h"
// #include "Game.h"
// #include "GameObject/GameObject.h"
// #include "Scene/Scene.h"
// #include "Component/Profiler/Profiler.h"
//
// #include <iostream>
//
// #include "AgentsZandbak.h"
// #include "IZandbak.h"
//
// // This has been added because sometimes SDL causes main to be redefined.
// // Which then causes linking error's
// #undef main
//
//
// int main(int argc, char** argv)
// {
// 	ApplicationSpecifications spec = {};
// 	spec.networkingOptions.port = 8080;
// 	spec.networkingOptions.serverIP = "127.0.0.1";
// 	spec.networkingOptions.mode = EngineMode::CLIENT;
// 	spec.networkingOptions.tickRate = 60;
// 	spec.renderBackend = RenderBackend::SDL;
// 	spec.windowOptions = {"GameEngine", 700, 700};
//
// 	std::unique_ptr<Game> spel = std::make_unique<Game>();
// 	// std::unique_ptr<Scene> scene = std::make_unique<Scene>("Scene");
//
// 	// Panel
// 	std::unique_ptr<GameObject> profiler = std::make_unique<GameObject>();
// 	profiler->addComponent<Profiler>(480.0f, 10.0f, 210.0f, 320.0f);  // Top-right of 700x700 window
//
//     /// Note: Change the unique_ptr to create the sandbox you want
//     std::unique_ptr<IZandbak> zandbak = std::make_unique<AgentsZandbak>();
// 	std::unique_ptr<Scene> scene = zandbak->getScene();
//
// 	scene->addGameObject(std::move(profiler));
// 	spel->addScene(std::move(scene));
// 	spel->setApplicationSpecifications(spec);
// 	return SpelMotorEntry::main(std::move(spel));
//  	spel->setApplicationSpecifications(spec);
//
//     return SpelMotorEntry::main(std::move(spel));
// }
