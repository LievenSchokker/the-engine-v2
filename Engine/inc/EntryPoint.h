#pragma once


#include "Game.h"
#include "Core/SpelMotor.h"

/// This has been added because sometimes SDL causes main to be redefined.
/// Which then causes linking error's
#undef main

namespace SpelMotorEntry
{
inline int main(std::unique_ptr<Game> game)
{
	SpelMotor engine(std::move(game));
	engine.start();
	return 0;
}
}