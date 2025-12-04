//
// Created by Lieven Schokker on 07/11/2025.
//

#include <iostream>

#include "EntryPoint.h"

#include "External/SdlContext.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"
#include "Component/Transform.h"
#include "Component/ShapeRenderer.h"
#include "GameObject/GameObject.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue.h"
#include "Input/InputManager.h"

#include <memory>
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
// This has been added because sometimes SDL causes main to be redefined.
// Which then causes linking error's
#undef main


int main(int argc, char** argv)
{
    return SpelMotorEntry::main(argc, argv);

    /// Make game cinstance
    /// shizzle
    ///
    /// SpelMotor->addGame(game);
    /// SpelMotor->run();
}
