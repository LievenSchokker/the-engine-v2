#pragma once
#include "Component/ShapeRenderer.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"
#include "GameObject/GameObject.h"

namespace SpelMotorEntry
{
	inline int main(int argc, char** argv)
	{
		ApplicationSpecifications specifications{};

		specifications.windowOptions = {"SpelMotor", 700, 700};
		specifications.renderBackend = RenderBackend::SDL;
		specifications.tickRate = 60;
		SpelMotor engine(specifications);

	    std::unique_ptr<GameObject> go = std::make_unique<GameObject>();
	    go->addComponent<ShapeRenderer>();

		engine.run();
		return 0;
	}
}
