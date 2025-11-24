#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"
#include "Physics/PhysicsSystem.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#include "EntryPoint.h"

int main(int argc, char** argv)
{
    return SpelMotorEntry::main(argc, argv);
}
