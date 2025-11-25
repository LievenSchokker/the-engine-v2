#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"
#include "Physics/IPhysicsWorld.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "GameObject/Vector2.h"

#include <SDL2/SDL.h>
#include <iostream>

#include "EntryPoint.h"

int main(int argc, char** argv)
{
    return SpelMotorEntry::main(argc, argv);
}
