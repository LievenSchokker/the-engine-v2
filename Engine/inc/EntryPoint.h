#pragma once
#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"

namespace SpelMotorEntry
{
	inline int main(int argc, char** argv)
	{
		ApplicationSpecifications specifications{};

		specifications.windowOptions = {"SpelMotor", 700, 700};
		specifications.renderBackend = RenderBackend::SDL;

		SpelMotor engine(specifications);
		engine.run();
		return 0;
	}
}
