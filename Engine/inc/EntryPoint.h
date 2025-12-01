#pragma once


#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"
#include "Networking/Client.h"


namespace SpelMotorEntry
{
	inline int main(int argc, char** argv)
	{
		ApplicationSpecifications specifications{};

		specifications.windowOptions = {"SpelMotor", 700, 700};
		specifications.renderBackend = RenderBackend::SDL;
		specifications.networkingOptions.tickRate = 60;
	    specifications.networkingOptions.mode = EngineMode::CLIENT;
	    specifications.networkingOptions.port = 8080;
	    specifications.networkingOptions.serverIP = "127.0.0.1";
		SpelMotor engine(specifications);
		engine.start();
		return 0;
	}
}
