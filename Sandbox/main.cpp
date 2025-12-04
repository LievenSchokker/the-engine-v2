//
// Created by Lieven Schokker on 07/11/2025.
//

#include <iostream>

#include "EntryPoint.h"

// This has been added because sometimes SDL causes main to be redefined.
// Which then causes linking error's
#undef main


int main(int argc, char** argv)
{
	ApplicationSpecifications spec = {};
	spec.tickRate = 60;
	spec.renderBackend = RenderBackend::SDL;
	spec.windowOptions = {"GameEngine", 700, 700};
	std::unique_ptr<Game> spel = std::make_unique<Game>();
	spel->setApplicationSpecifications(spec);
    return SpelMotorEntry::main(std::move(spel));

}
