//
// Created by Lieven Schokker on 07/11/2025.
//

#include "EntryPoint.h"
#include "Scene/SceneManager.h"

// This has been added because sometimes SDL causes main to be redefined.
// Which then causes linking error's
#undef main


int main(int argc, char** argv)
{
    SceneManager sceneManager = SceneManager();

    return SpelMotorEntry::main(argc, argv);
}