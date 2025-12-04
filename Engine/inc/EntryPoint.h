#pragma once


#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"

/// This has been added because sometimes SDL causes main to be redefined.
/// Which then causes linking error's
#undef main

namespace SpelMotorEntry
{

	inline int main(int argc, char** argv)
	{
		ApplicationSpecifications specifications{};

		specifications.windowOptions = {"SpelMotor", 700, 700};
		specifications.renderBackend = RenderBackend::SDL;
		specifications.tickRate = 60;
		SpelMotor engine(specifications);
		engine.start();
		return 0;
	}
}
