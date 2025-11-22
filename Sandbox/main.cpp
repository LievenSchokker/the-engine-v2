#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"
#include "box2d/box2d.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"

#include <SDL2/SDL.h>
#include <iostream>

#include "EntryPoint.h"

int main(int argc, char** argv)
{
    return SpelMotorEntry::main(argc, argv);
}
